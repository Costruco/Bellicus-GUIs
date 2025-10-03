#include <SDL2/SDL.h>

#include "espera.h"

int AUX_WaitEventTimeoutCount(SDL_Event* evt, Uint32 * ms) {
	Uint32 antes = SDL_GetTicks();
	int isevt = SDL_WaitEventTimeout(evt, *ms);
	Uint32 deltaEspera = SDL_GetTicks()-antes, novaEspera = *ms-deltaEspera;
	if (novaEspera > *ms)
		*ms = 0;
	else
		*ms -= deltaEspera;
	return isevt;
}
