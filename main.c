/*
 * ATENCAO: NAO ALTERAR ESTE ARQUIVO
 * Seu trabalho deve ser desenvolvido no arquivo memory_manager.c
 * na funcao memory_access(int addr, int *data, int type).
 *
 * Para compilar o projeto execute o comando
 *     make
 * Para executar digite o comando:
 *    ./main
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include "memory_manager.h"

void fill_memory() {
	int byte_addr;
	for (byte_addr = 0; byte_addr < RAM_SIZE; byte_addr++) {
		memory[byte_addr] = 0;
	}
}

void fill_cache(cache * c, int set_size, int block_size, int word_size) {
	int s;
	int b;
	int w;
	for (s = 0; s < set_size; s++) {
		for (b = 0; b < block_size; b++) {
			c[s].blocks[b].valid = 0;
			c[s].blocks[b].modified = 0;
			c[s].blocks[b].subst = 0;
			c[s].blocks[b].tag = 0;
			for (w = 0; w < word_size; w++) {
				c[s].blocks[b].words[w] = 0;
			}
		}
	}
}

/*
 * Assumindo que estamos utilizando big endian
 * o byte mais significativo eh o byte 0 e o
 * menos significativo eh o byte 3.
 */
unsigned int byte_to_int(unsigned char bytes[]) {
	return (unsigned int)((unsigned int)bytes[0] << 24) + ((unsigned int)bytes[1] << 16) +
			((unsigned int)bytes[2] << 8) + ((unsigned int)bytes[3]);
}

void print_memory() {
	int byte_addr;
	int word_addr;
	int block;
	int unsigned content;

	printf("------------ MEMORIA -------------\n");
	printf("Block\tWord\tByte\tContent\n");
	printf("----------------------------------\n");

	for (byte_addr = 0; byte_addr < RAM_SIZE; byte_addr = byte_addr + 4) {
		unsigned char word_byte[4] = { memory[byte_addr], memory[byte_addr + 1],
				memory[byte_addr + 2], memory[byte_addr + 3]};

		word_addr = byte_addr/4;
		content = byte_to_int(word_byte);

		if (word_addr % WORDS_L2 == WORDS_L2/2) {
			block = word_addr / WORDS_L2;
			printf("%d\t%d\t%d\t%10u\n", block, word_addr, byte_addr, content);
		} else {
			printf("\t%d\t%d\t%10u\n", word_addr, byte_addr, content);
			if (word_addr % WORDS_L2 == WORDS_L2 - 1) {
				printf("----------------------------------\n");
			}
		}
	}
}

void print_cache_l1() {
	int set;
	int block;
	int word;
	printf("%53s\n","CACHE L1");
	printf("|-------------------- BLOCK 0 -------------------|");
	printf("|-------------------- BLOCK 1 -------------------|\n");
	printf("| Set | v | subst | tag |     w0     |     w1    |");
	printf("| Set | v | subst | tag |     w0     |     w1    |\n");
	for (set = 0; set < SETS_L1; set++) {
		for (block = 0; block < BLOCKS_L1; block++) {
			int valid = cache_L1[set].blocks[block].valid;
			int subst = cache_L1[set].blocks[block].subst;
			int tag = cache_L1[set].blocks[block].tag;
			printf("|  %d  | %d |   %d   | %03d | ", set, valid, subst, tag);
			for (word = 0; word < WORDS_L1; word++) {
				int w = cache_L1[set].blocks[block].words[word];
				printf("%10u |", w);
			}
		}
		printf("\n");
	}
	printf("|------------------------------------------------|");
	printf("|------------------------------------------------|\n");
}

void print_cache_l2() {
	int set;
	int block;
	int word;
	printf("%65s\n","CACHE L2");
	printf("|-------------------------- BLOCK 0 -------------------------|");
	printf("|-------------------------- BLOCK 1 -------------------------|\n");
	printf("| Set | v | m | subst | tag |   w0   |   w1  |   w2  |   w3  |");
	printf("| Set | v | m | subst | tag |   w0   |   w1  |   w2  |   w3  |\n");
	for (set = 0; set < SETS_L2; set++) {
		for (block = 0; block < BLOCKS_L2; block++) {
			int valid = cache_L2[set].blocks[block].valid;
			int modified = cache_L2[set].blocks[block].modified;
			int subst = cache_L2[set].blocks[block].subst;
			int tag = cache_L2[set].blocks[block].tag;
			printf("|  %d  | %d | %d |   %d   | %03d | ", set, valid, modified, subst, tag);
			for (word = 0; word < WORDS_L2; word++) {
				int w = cache_L2[set].blocks[block].words[word];
				printf("%6u |", w);
			}
		}
		printf("\n");
	}
	printf("|------------------------------------------------------------|");
	printf("|------------------------------------------------------------|\n");
}

int allocate_cache(cache ** c, int set_size, int block_size, int word_size) {
	int s, b;
	*c = (cache*) malloc(sizeof(cache) * set_size);
	for(s = 0; s < set_size; s++) {
		(*c)[s].blocks = (block*) malloc(sizeof(block) * block_size);
		for (b = 0; b < block_size; b++) {
			(*c)[s].blocks[b].words = (unsigned int *) malloc(sizeof(unsigned int) * word_size);
		}
	}
}

int main(int argc, char *argv[]) {
	allocate_cache(&cache_L1, SETS_L1, BLOCKS_L1, WORDS_L1);
	cache_L1[0].blocks[0].words[0] = 1;
	allocate_cache(&cache_L2, SETS_L2, BLOCKS_L2, WORDS_L2);
	fill_memory();
	fill_cache(cache_L1, SETS_L1, BLOCKS_L1, WORDS_L1);
	fill_cache(cache_L2, SETS_L2, BLOCKS_L2, WORDS_L2);

	int exit = 0;
	int addr;
	int data;
	int type;
	int success;



	/*
	 *
	 * teste
	 */


	/*escrevendo valor na cache pra ser
	 * testado
	 */
	//end 200
	/*
	cache_L1[0].blocks[0].tag = 25;
	cache_L1[0].blocks[0].words[0] = 14;

	//end 201
	cache_L1[0].blocks[0].tag = 25;
	cache_L1[0].blocks[0].words[1] = 23;

	//inserimos dados na L2
	cache_L2[2].blocks[0].tag = 6;
	cache_L2[2].blocks[0].words[0] = 140;
	cache_L2[2].blocks[0].words[1] = 141;
	cache_L2[2].blocks[0].words[2] = 142;
	cache_L2[2].blocks[0].words[3] = 143;

	//em L2 add 228-231 (set 1 e tag 7)
	cache_L2[1].blocks[0].tag = 7;
	cache_L2[1].blocks[0].words[0] = 2280;
	cache_L2[1].blocks[0].words[1] = 2290;
	cache_L2[1].blocks[0].words[2] = 2300;
	cache_L2[1].blocks[0].words[3] = 2310;
	*/


	//fim do teste


	while (!exit) {
		printf("\n\nEndereco de acesso (palavra: 0-255) (-1 encerra o programa): ");
		scanf("%d", &addr);
		printf("\n");

		if (addr == -1) {
			exit = 1;
			continue;
		}

		printf("Informe o tipo do acesso ( 0 - Leitura, 1 - Escrita): ");
		scanf("%d", &type);
		printf("\n");

		if (type != 0 && type != 1) {
			printf("Tipo invalido.");
			continue;
		}

		if (type == 1) {
			printf("Informe o dado a ser escrito: ");
			scanf("%d", &data);
			printf("\n");
		}

		success = memory_access(addr, &data, type);

		if (success == 1) {
			if (type == 1) printf("O dado %d foi inserido na posicao %d com sucesso.\n", data, addr);
			else printf("O dado do endereco %d eh %d\n", addr, data);
		} else printf("Nao foi possivel realizar o acesso a memoria.\n");
	}

	print_memory();
	print_cache_l1();
	print_cache_l2();
}
