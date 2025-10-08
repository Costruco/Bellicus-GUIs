#ifndef ESTADOS_H
#define ESTADOS_H

#include <string.h>

enum movimento {
	PONTO_MORTO,
	TRANSV,
	REV,
	ESQ_TRANSV,
	DIR_TRANSV,
	ESQ_REV,
	DIR_REV,
	ESQ,
	DIR	
};

void stateToString(char * string, movimento estado);

#endif
