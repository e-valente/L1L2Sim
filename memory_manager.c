/*
 * ATENCAO: NAO ALTERAR OS ARQUIVO "main.c" e "memory_manager.h"
 * Seu trabalho deve ser desenvolvido neste arquivo "memory_manager.c"
 * a partir da funcao memory_access(int addr, int *data, int type).
 *
 * Para compilar o projeto execute o comando
 *     make
 * Para executar digite o comando:
 *    ./main
 *
 * */

#include <stdlib.h>
#include <stdio.h>
#include "memory_manager.h"

int memory_access(int addr, int *data, int type) {
	/* Seu codigo comeca aqui :) */

	int ret;
	unsigned char bytes[4];
	int i;
	int num = *data;

	if(type == 0)
	{
		printf("Leitura....\n");

		/*escrevendo valor na cache pra ser
		 * testado
		 */
		cache_L1[0].blocks[0].tag = 25;
		cache_L1[0].blocks[0].words[0] = 14;

		cache_L2[2].blocks[0].tag = 6;
		cache_L2[2].blocks[0].words[0] = 140;

		//fim do teste


		//tenta ler em L1
		ret = readMemory(addr, data, L1);

		//se sucesso ao ler em L1
		if(ret == 1){
			printf("\ndata vale %d\n", *data);
			return 1;
		}


		//tenta ler em L2
		//ret = readMemory(addr, data, L2);


	}


	if(type ==1)
	{
		printf("Escrita..\n");

		//escreve em memória cache,
		//iniciando pelo nível L1
		ret = writeMemory(addr, data, L1);

		for(i = 0; i < 4; i++)
		{
			bytes[i] = num & 0xFF;
			printf("byte %d = %d\n", i, bytes[i]);
			num >>= 8;

		}

		i = addr * 4;
		memory[i] = bytes[3];
		memory[1+1] = bytes[2];
		memory[i+2] = bytes[1];
		memory[i+3] = bytes[0];

	}


	return -1;
}


int readMemory(int addr, int *data, int cachelevel)
{
	int set, tag, word_offset;
	int i, ret;

	//lendo em L1
	if(cachelevel == L1)
	{
		printf("buscando em L1");
		//obtem tag, set e word_offset
		tag = addr >> 3;
		set = (addr & 0x06) >> 1;
		word_offset = addr & 0x01;

		//printf("tag %d set %d word_offset: %d\n", tag, set, word_offset);

		//procura a tag no conjunto
		for(i = 0; i < BLOCKS_L1; i++)
		{
			if(cache_L1[set].blocks[i].tag == tag)
			{
				//escrevemos em data o valor da word desejada
				*data = cache_L1[set].blocks[i].words[word_offset];

				return 1;
			}
		}


		//se nao tiver em L1, buscamos em L2 e gravamos em L1
		ret = readMemory(addr, data, L2);

		if(ret == 1)
		{
			printf("achei em L2\n");
			printf("preciso escrever em L1\n");
			//pra escrever, terei que usar
			//a funcao que criarei na escrita

		}

		//se nao está em L2, lemos da RAM e gravamos em L2
		if(ret == 0)
		{
			ret = readMemory(addr, data, RAM);

		}



		return 1;

	}

	//lendo em L2
	if(cachelevel == L2)
	{
		printf("buscando em L2\n");
		//obtem tag, set e word_offset
		tag = addr >> 5;
		set = (addr & 0x1c) >> 2;
		word_offset = addr & 0x03;

		printf("\nEm L2 tag %d set %d word_offset: %d\n", tag, set, word_offset);

		//procura a tag no conjunto
		for(i = 0; i < BLOCKS_L2; i++)
		{
			if(cache_L2[set].blocks[i].tag == tag)
			{
				//escrevemos em data o valor da word desejada
				*data = cache_L2[set].blocks[i].words[word_offset];

				return 1;
			}
		}

	}

	//lendo da RAM
	if(cachelevel == RAM)
	{

	}



	return -1;

}



int writeMemory(int addr, int *data, int cachelevel)
{
	int set, tag, word_offset;
	int i, ret;
	unsigned char word_array[4];

	//escreve em L1
	if(cachelevel == L1)
	{
		//obtem tag, set e word_offset
		tag = addr >> 3;
		set = (addr & 0x06) >> 1;
		word_offset = addr & 0x01;

		//printf("tag %d set %d word_offset: %d\n", tag, set, word_offset);

		//procura a tag no conjunto
		for(i = 0; i < BLOCKS_L1; i++)
		{
			//caso esteja em L1
			if(cache_L1[set].blocks[i].tag == tag)
			{
				//escrevemos a palavra desejada em L1
				cache_L1[set].blocks[i].words[word_offset] = *data;

				//L1 é write through, portanto devemos
				//imediatamente escrever em L2
				ret = writeMemory(addr, data, L2);
				return ret;
			}
		}


		//se nao tiver, temos que carregar de de L2
		//ret = readMemory(addr, data, L2);
	}




	/*Escreve no nível L2*/
	if(cachelevel == L2)
	{
		//obtem tag, set e word_offset
		tag = addr >> 5;
		set = (addr & 0x1c) >> 2;
		word_offset = addr & 0x03;

		//printf("tag %d set %d word_offset: %d\n", tag, set, word_offset);

		//procura a tag no conjunto
		for(i = 0; i < BLOCKS_L2; i++)
		{
			//caso esteja em L1
			if(cache_L2[set].blocks[i].tag == tag)
			{
				//escrevemos a palavra desejada em L2
				cache_L2[set].blocks[i].words[word_offset] = *data;

				//testa gravacao WT na ram
				return writeMemory(addr, data, RAM);

				return 1;

			}
		}


		//se bloco nao estiver em L2, devemos carrega-lo
		//da ram e depois escrever nele
	}




	/*Escreve word na RAM*/
	if(cachelevel == RAM)
	{
		/*precisamos dividir a word (*data)
		 * em 4 bytes. Cada byte será armazenado no vetor
		 * word_array
		 */

		for(i = 0; i < 4; i++)
		{
			word_array[i] = *data & 0xFF;
			printf("byte %d = %d\n", i, word_array[i]);
			*data >>= 8;

		}

		//i recebe o offset da word (a cada 4 bytes temos 1 word)
		i = addr * 4;

		/*armazena byte na memória  (em big endian)*/

		memory[i] = word_array[3];
		memory[1+1] = word_array[2];
		memory[i+2] = word_array[1];
		memory[i+3] = word_array[0];

		return 1;

	}


	return -1;

}

