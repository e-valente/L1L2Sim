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

	unsigned char bytes[4];
	int i;
	int num = *data;

	if(type == 0)
		printf("Leitura....\n");
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
