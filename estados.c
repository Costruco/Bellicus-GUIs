#include "estados.h"

void movimentoToString(char * string, Movimento estado) {
	switch (estado) {
		case NULO:
			strcpy(string,"NULO");
			break;
		case TRANSV:
			strcpy(string,"TRANSV");
			break;
		case REV:
			strcpy(string,"REV");
			break;
	}
}

void direcaoToString(char * string, Direcao estado) {
	switch (estado) {
		case RETO:
			strcpy(string,"RETO");
			break;
		case TRANSV:
			strcpy(string,"ESQ");
			break;
		case REV:
			strcpy(string,"DIR");
			break;
	}
}

void marchaToString(char * string, Marcha estado) {
	switch (estado) {
		case RE:
			strcpy(string,"RE");
			break;
		case PONTO_MORTO:
			strcpy(string,"PONTO_MORTO");
			break;
		case PRIMEIRA:
			strcpy(string,"PRIMEIRA");
			break;
		case SEGUNDA:
			strcpy(string,"SEGUNDA");
			break;
		case TERCEIRA:
			strcpy(string,"TERCEIRA");
			break;
		case QUARTA:
			strcpy(string,"QUARTA");
			break;
	}
}
