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

#ifndef MEMORY_MANAGER_H_INCLUDED
#define MEMORY_MANAGER_H_INCLUDED

#define RAM_SIZE 1024     // A RAM possui 256 words dividida em 1024 bytes (4 bytes por word)
#define SETS_L1 4         // Quantidade de conjuntos para cache L1
#define SETS_L2 8         // Quantidade de conjuntos para cache L2
#define BLOCKS_L1 2       // Quantidade de blocos para cache L1
#define BLOCKS_L2 2       // Quantidade de blocos para cache L2
#define WORDS_L1 2        // Quantidade de palavras da cache L1
#define WORDS_L2 4        // Quantidade de palavras da cache L2

//constantes que serao usadas no acesso à memoria
#define L1 1
#define L2 2
#define RAM 3


typedef struct {
  int valid;              // bit de validade
  int modified;           // bit de modificado
  int subst;              // bit de controle para politica de substituicao
  int tag;                // tag do bloco
  unsigned int * words;    // palavras do bloco
} block;

typedef struct {
  block * blocks;
} cache;

unsigned char memory[RAM_SIZE];    // char representa 1 byte de 8 bits (desconsidera bit de sinal)
cache* cache_L1;
cache* cache_L2;

int memory_access(int addr, int *data, int type);
int readMemory(int addr, int *data, int cachelevel);
int writeMemory(int addr, int *data, int cachelevel);

/*verifica se a tag esta na cache de nível cachelevel*/
int isTagOnCache(int addr, int cachelevel);

/*le a palavra na cache
 * ATENCAO: Essa funcao só é usada para ler
 * o nível mais alto (perto do processador)
 * Neste trabalho é o nível L1
 */
int readWord(int addr, int *data, int cachelevel);

int writeWord(int addr, int *data, int cachelevel);


/*carrega conjunto de words do nível cachelevel_src
 * para o nível cachelevel_dst
 */
int loadSetOfWordsOnCache(int addr, int cachelevel_src, int cachelevel_dst);

/*Escreve o conjunto de words (array words*) do cache nível
 * cachelevelsrc  para o cache nível cachelevel_dst
 *
 */


int wichBlock(int addr, int cachelevel);

void parseAddr(int addr, int cachelevel, int *tag, int *set, int *word_offset);
int getRAMWordFromBlock(int addr, int word_offset);


#endif
