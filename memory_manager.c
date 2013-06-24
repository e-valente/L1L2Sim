/*Grupo 5
*Turma2 - Grupo5
*
*Alunos:
*Bruno Azenha
*Emanuel Valente
*Vinícius Katata
*
* * */

#include <stdlib.h>
#include <stdio.h>
#include "memory_manager.h"

/*constantes que serao usadas
 * no acesso à memoria
 */

#define L1 1
#define L2 2
#define RAM 3


/*Protótipos das funcoes */
int memory_access(int addr, int *data, int type);
int readMemory(int addr, int *data, int cachelevel);
int writeMemory(int addr, int *data, int cachelevel);
int isTagOnCache(int addr, int cachelevel);
int readWord(int addr, int *data, int cachelevel);
int writeWord(int addr, int *data, int cachelevel);
int loadSetOfWordsOnCache(int addr, int cachelevel_src, int cachelevel_dst);
int wichBlockFree(int addr, int cachelevel);
int wichBlockContainTag(int addr, int cachelevel);
void parseAddr(int addr, int cachelevel, int *tag, int *set, int *word_offset);
int getRAMWordFromBlock(int addr, int word_offset);


/*Inicio das implementações*/

int memory_access(int addr, int *data, int type) {
	/* Seu codigo comeca aqui :) */

	unsigned char bytes[4];
	int i;
	int num = *data;

	if(type == 0)
	{
		//comeca a ler do nível mais alto
		return readMemory(addr, data, L1);
	}


	if(type ==1)
	{
		//escreve em memória cache,
		//iniciando pelo nível L1
		return writeMemory(addr, data, L1);
	}

	return -1;
}

/*Le palavra a partir do nível cachelevel
 * Se a palavra nao estiver no nível atual, a rotina busca
 * o bloco no  nível abaixo de de maneira recursiva
 * até a memoria principal, caso  seja necessário.
 * Retorna 1 em sucesso e -1 se houver falha.
 */
int readMemory(int addr, int *data, int cachelevel)
{
	//lendo em L1
	if(cachelevel == L1)
	{


		if(isTagOnCache(addr, L1) == 1)
		{
			*data = readWord(addr, data, L1);
			return 1;
		}

		/*tag nao está na cache
		 * carregamos o bloco de L2 em L1
		 */

		//se a tag está em L2, deve ser carregada em L1
		if(isTagOnCache(addr, L2) == 1)
		{
			loadSetOfWordsOnCache(addr, L2, L1);
			*data = readWord(addr, data, L1);
			return 1;
		}

		//tag nao esta em L1 e nem em L2
		//carregamos da RAM para L2 e de L2 para L1
		loadSetOfWordsOnCache(addr, RAM, L2);
		loadSetOfWordsOnCache(addr, L2, L1);

		//le word em L1
		*data = readWord(addr, data, L1);
		return 1;


	}




	return -1;

}


/*Escreve palavra a partir do nível cachelevel
 * Se a palavra nao estiver no nível atual, a rotina busca
 * o bloco no  nível abaixo de de maneira recursiva
 * até a memoria principal, caso  seja necessário.
 * Retorna 1 em sucesso e -1 se houver falha.
 */
int writeMemory(int addr, int *data, int cachelevel)
{
	//lendo em L1
	if(cachelevel == L1)
	{

		//printf("tag %d set %d word_offset: %d\n", tag, set, word_offset);
		if(isTagOnCache(addr, L1) == 1)
		{
			return writeWord(addr, data, L1);
		}

		//se a word nao estiver em L1,
		//buscamos em L2. Caso esteja, carregamos o conjunto
		//de L2 para L1 e gravamos a word em L1
		if(isTagOnCache(addr, L2) == 1)
		{
			loadSetOfWordsOnCache(addr, L2, L1);
			return writeWord(addr, data, L1);

		}

		/*tag nao está em L2, portanto,
		 * temos que carregar da RAM para L2
		 * e de L2 para L1 e entao gravar word em L1
		 */

		loadSetOfWordsOnCache(addr, RAM, L2);
		loadSetOfWordsOnCache(addr, L2, L1);
		return writeWord(addr, data, L1);


	}


	return -1;

}



/*somente escreve word do nível L1*/
int writeWord(int addr, int *data, int cachelevel)
{
	int set, tag, word_offset;
	int i;

	//escrevendo em L1
	if(cachelevel == L1)
	{

		//obtem tag, set e word_offset de L1
		parseAddr(addr, L1, &tag, &set, &word_offset);

		//laco pra procurar em qual bloco escrever
		for(i = 0; i < BLOCKS_L1; i++)
		{
			if(cache_L1[set].blocks[i].tag == tag)
			{
				//seta tag validade
				cache_L1[set].blocks[i].valid = 1;
				cache_L1[set].blocks[i].words[word_offset] = *data;
				/*politica de L1 é WT, portanto, garantimos
				 * que o tag está em L2
				 */
				return writeWord(addr, data, L2);
			}

		}

		return -1;

	}


	//escrevendo em L2
	if(cachelevel == L2)
	{
		//obtem tag, set e word_offset de L1
		parseAddr(addr, L2, &tag, &set, &word_offset);

		//laco pra procurar em qual bloco escrever
		for(i = 0; i < BLOCKS_L2; i++)
		{
			if(cache_L2[set].blocks[i].tag == tag)
			{
				//seta o bit de validade e modificado (m)

				cache_L2[set].blocks[i].valid = 1;
				cache_L2[set].blocks[i].modified = 1;
				cache_L2[set].blocks[i].words[word_offset] = *data;
				return 1;
			}

		}

		return -1;

	}


	return -1;

}

/*carrega conjunto de words do nível cachelevel_src
 * para o nível cachelevel_dst
 */
int loadSetOfWordsOnCache(int addr, int cachelevel_src, int cachelevel_dst)
{
	int set_src, tag_src, word_offset_src;
	int set_dst, tag_dst, word_offset_dst;
	int block_src, block_dst;
	int i, j, pos_ini, myword;
	unsigned char bytes[4];



	/* words devem ir de L2 para L1*/
	if(cachelevel_src == L2 && cachelevel_dst == L1)
	{
		//calcula tag, set e word_offset de L1
		parseAddr(addr, L1, &tag_dst, &set_dst, &word_offset_dst);

		//calcula tag, set e word_offset de L2
		parseAddr(addr, L2, &tag_src, &set_src, &word_offset_src);

		//posicao inicial depende dos offsets
		if(word_offset_src >= 2) pos_ini = 2;
		else pos_ini = 0;

		/*L1 usa política de substituicao FIFO
		 * Verifica se conjunto está vazio pelo bit
		 * de validade
		 * */

		block_src = wichBlockContainTag(addr, L2);
		block_dst = wichBlockFree(addr, L1);

		//bloco livre é o da direta, portanto
		//podemos excluir o da direita da proxima vez
		//setando o bit de substituicao
		if(block_dst > 1)
		{
			cache_L1[set_dst].blocks[0].subst = 1;

		}

		//grava tag
		cache_L1[set_dst].blocks[block_dst].tag = tag_dst;
		//seta bit validade
		cache_L1[set_dst].blocks[block_dst].valid = 1;

		//zera bit de substituicao
		cache_L1[set_dst].blocks[block_dst].subst = 0;


		for(j = 0; j < WORDS_L1; j++)
		{
			cache_L1[set_dst].blocks[block_dst].words[j] = cache_L2[set_src].blocks[block_src].words[pos_ini + j];


		}

		return 1;


	}



	/* words devem ir de RAM para L2*/
	if(cachelevel_src == RAM && cachelevel_dst == L2)
	{
		//calcula tag, set e word_offset de L2
		parseAddr(addr, L2, &tag_dst, &set_dst, &word_offset_dst);

		//fprintf(stderr,"word_offset_src: %d word_offset_dst %d pos_ini %d\n", word_offset_src, word_offset_dst, pos_ini);
		//return 1;

		//block_src = wichBlock(addr, L2);
		block_dst = wichBlockFree(addr, L2);


		//bloco livre é o da direta, portanto
		//podemos excluir o da direita da proxima vez
		//setando o bit de substituicao
		if(block_dst > 1)
		{
			cache_L2[set_dst].blocks[0].subst = 1;

		}


		/*Antes de escrever, devemos verificar
		 * se o bit M está modificado. Caso esteja,
		 * devemos escrever o conteudo de L2 pra RAM
		 */
		if(cache_L2[set_dst].blocks[block_dst].modified == 1)
		{
			loadSetOfWordsOnCache(addr, L2, RAM);

			//limpa bit modificado
			cache_L2[set_dst].blocks[block_dst].modified = 0;


		}

		//grava tag
		cache_L2[set_dst].blocks[block_dst].tag = tag_dst;

		//grava bit de validade
		cache_L2[set_dst].blocks[block_dst].valid = 1;


		//zera bit de substituicao
		cache_L2[set_dst].blocks[block_dst].subst = 0;

		for(j = 0; j < WORDS_L2; j++)
		{
			printf("escrevendo em L2: %d\n",  getRAMWordFromBlock(addr, j));
			cache_L2[set_dst].blocks[block_dst].words[j] = getRAMWordFromBlock(addr, j);


		}

		return 1;


	}



	if(cachelevel_src == L2 && cachelevel_dst == RAM)
	{

		//calcula tag, set e word_offset de L2
		parseAddr(addr, L2, &tag_src, &set_src, &word_offset_src);

		block_src = wichBlockContainTag(addr, L2);

		//le bloco
		for(i = 0; i < WORDS_L2; i++)
		{
			myword = cache_L2[set_src].blocks[block_src].words[i];
			//laco pra configurar a palavra (dividir em 4 bytes)
			//a ser gravada no vetor da memoria
			for(j = 0; j < 4; j++)
			{
				bytes[j] = myword & 0xFF;
				//printf("byte %d = %d\n", j, bytes[j]);
				myword >>= 8;

			}

			j = (addr + i) * 4;
			memory[j] = bytes[3];
			memory[j+1] = bytes[2];
			memory[j+2] = bytes[1];
			memory[j+3] = bytes[0];

		}

	}



	return -1;


}



int wichBlockFree(int addr, int cachelevel)
{
	int set, tag, word_offset;
	int i;

	//lendo em L1
	if(cachelevel == L1)
	{

		//obtem tag, set e word_offset de L1
		parseAddr(addr, L1, &tag, &set, &word_offset);

		for(i = 0; i < BLOCKS_L1; i++)
		{
			if(cache_L1[set].blocks[i].valid == 0 ||
					cache_L1[set].blocks[i].subst == 1) return i;

		}

		return -1;

	}


	//lendo em L2
	if(cachelevel == L2)
	{

		//obtem tag, set e word_offset de L1
		parseAddr(addr, L2, &tag, &set, &word_offset);


		for(i = 0; i < BLOCKS_L2; i++)
		{
			if(cache_L2[set].blocks[i].valid == 0 ||
					cache_L2[set].blocks[i].subst == 1) return i;

		}

		return -1;

	}



	return -1;

}


int wichBlockContainTag(int addr, int cachelevel)
{
	int set, tag, word_offset;
	int i;

	//lendo em L1
	if(cachelevel == L1)
	{

		//obtem tag, set e word_offset de L1
		parseAddr(addr, L1, &tag, &set, &word_offset);

		for(i = 0; i < BLOCKS_L1; i++)
		{
			if(cache_L1[set].blocks[i].tag == tag) return i;
		}

	}


	//lendo em L2
	if(cachelevel == L2)
	{

		//obtem tag, set e word_offset de L1
		parseAddr(addr, L2, &tag, &set, &word_offset);


		for(i = 0; i < BLOCKS_L2; i++)
		{
			if(cache_L2[set].blocks[i].tag == tag) return i;
		}

	}



	return -1;

}


void parseAddr(int addr, int cachelevel, int *tag, int *set, int *word_offset)
{

	if(cachelevel == L1)
	{
		//calcula tag, set e word_offset de L1
		*tag = addr >> 3;
		*set = (addr & 0x06) >> 1;
		*word_offset = addr & 0x01;
	}

	if(cachelevel == L2)
	{
		//calcula tag, set e word_offset de L2
		*tag = addr >> 5;
		*set = (addr & 0x1c) >> 2;
		*word_offset = addr & 0x03;
	}

	if(cachelevel == RAM)
	{
		//calcula tag, set e word_offset de L2
		*tag = -1;
		//set é o bloco da memoria
		*set = (addr & 0xfc) >> 2;
		*word_offset = addr & 0x03;
	}
}



/*verifica se a tag esta na cache
 * de nível cachelevel*/
int isTagOnCache(int addr, int cachelevel)
{

	int set, tag,  word_offset;
	int i;// ret;

	//lendo em L1
	if(cachelevel == L1)
	{

		//obtem tag, set e word_offset de L1
		parseAddr(addr, L1, &tag, &set, &word_offset);

		//procura a tag no conjunto
		for(i = 0; i < BLOCKS_L1; i++)
		{
			if(cache_L1[set].blocks[i].tag == tag)
				return 1;
		}

		return 0;

	}


	//lendo em L2
	if(cachelevel == L2)
	{

		//obtem tag, set e word_offset de L2
		parseAddr(addr, L2, &tag, &set, &word_offset);

		//procura a tag no conjunto
		for(i = 0; i < BLOCKS_L2; i++)
		{
			if(cache_L2[set].blocks[i].tag == tag)
				return 1;
		}

		return 0;

	}


	return -1;
}


/*le a palavra na cache
 * ATENCAO: Essa funcao só é usada para ler
 * o nível mais alto (perto do processador)
 * Neste trabalho é o nível L1
 */
int readWord(int addr, int *data, int cachelevel)
{
	int set, tag, word_offset;
	int i;

	//lendo em L1
	if(cachelevel == L1)
	{

		//obtem tag, set e word_offset de L1
		parseAddr(addr, L1, &tag, &set, &word_offset);

		//laco pra procurar em qual bloco ler
		for(i = 0; i < BLOCKS_L1; i++)
		{
			if(cache_L1[set].blocks[i].tag == tag)
			{
				//seta tag validade
				cache_L1[set].blocks[i].valid = 1;
				return cache_L1[set].blocks[i].words[word_offset];
			}
		}

		return -1;

	}

	else fprintf(stderr, "readWord() somente lê no nível L1\n");

	return -1;
}

int getRAMWordFromBlock(int addr, int word_offset)
{
	int index, myword;
	unsigned char bytes[4];

	//word_offset nao interessa
	addr = (addr & 0xfc) >> 2;

	//obtem primeira palavra do bloco
	index = addr * 4;
	//botem a palavra desejada
	index = index + word_offset;

	//obtem a primeira posicao da palvra no vetor memoria
	index = index * 4;


	bytes[0] = memory[index];
	bytes[1] = memory[index + 1];
	bytes[2] = memory[index + 2];
	bytes[3] = memory[index + 3];

	myword = ((unsigned int)bytes[0] << 24) + ((unsigned int)bytes[1] << 16) +
			((unsigned int)bytes[2] << 8) + ((unsigned int)bytes[3]);


	return myword;


}

