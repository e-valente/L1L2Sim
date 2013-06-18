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

		//teste
		printf("gravando em L1..\n");
		cache_L1[0].blocks[0].words[0] = 10;
		cache_L1[0].blocks[0].tag = 3;


		ret = read_from_l1(addr);

	}


	if(type ==1)
	{
		printf("Escrita..\n");

		for(i = 0; i < 4; i++)
		{
			bytes[i] = num & 0xFF;
			printf("byte %d = %d\n", i, bytes[i]);
			num >>= 8;

		}
		memory[0] = bytes[3];
		memory[1] = bytes[2];
		memory[2] = bytes[1];
		memory[3] = bytes[0];

	}


	return -1;
}

int read_from_l1(int addr)
{
	//unsigned char tag[3];
	int set, tag, word_offset;
	int ret, ret_tag, ret_set, ret_block, ret_offset;


	//obtem tag, set e word_offset
	tag = addr >> 5;
	set = (addr & 24) >> 3;
	word_offset = (addr & 0x04) >> 2;


	//pode procurar a palavra toda
	if(search_tag_and_set_on_l1(tag, set) == 1)
	{
		if(ret_set == set)
		{
			//procurar word
			return cache_L1[set].blocks[0].words[0];
		}

	}



	printf("tag vale %d set vale %d word offset vale %d\n", tag,set, word_offset);

	return 0;



}

int search_tag_and_set_on_l1(int tag, int set)
{

	int b;


	for(b = 0; b < BLOCKS_L1; b++)
	{
		if(cache_L1[set].blocks[b].tag == tag)
		{
			return 1;
		}
	}



	return 0;


}
