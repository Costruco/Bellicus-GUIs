#include <string.h>

#include "estados.h"

void stateToString(char * string, enum movimento estado) {
	switch (estado) {
		case PONTO_MORTO:
			strcpy(string,"PONTO_MORTO");
			break;
		case TRANSV:
			strcpy(string,"TRANSV");
			break;
		case REV:
			strcpy(string,"REV");
			break;
	    case ESQ_TRANSV:
			strcpy(string,"ESQ_TRANSV");
			break;
		case DIR_TRANSV:
			strcpy(string,"DIR_TRANSV");
			break;
		case ESQ_REV:
			strcpy(string,"ESQ_REV");
			break;
		case DIR_REV:
			strcpy(string,"DIR_REV");
			break;
		case ESQ:
			strcpy(string,"ESQ");
			break;
		case DIR:
			strcpy(string,"DIR");
			break;
	}
}
