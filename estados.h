#ifndef ESTADOS_H
#define ESTADOS_H

#include <string.h>

typedef enum Direcao {
	RETO,
	ESQ,
	DIR
} Direcao;

typedef enum Movimento {
	NULO,
	TRANSV,
	REV,
} Movimento;

typedef enum Marcha {
	RE,
	PONTO_MORTO,
	PRIMEIRA,
	SEGUNDA,
	TERCEIRA,
	QUARTA
} Marcha;

typedef enum LeitorDeFormato {
	FORA_DO_FORMATO,
	LENDO_FORMATO,
	SAINDO_DA_LEITURA
} LeitorDeFormato;

typedef enum EstadoDoJogo { 
	JOGO_VIVO, 
	JOGO_MORTO
} EstadoDoJogo;

void movimentoToString(char * string, Movimento estado);
void direcaoToString(char * string, Direcao estado);
void marchaToString(char * string, Marcha estado);

#endif
