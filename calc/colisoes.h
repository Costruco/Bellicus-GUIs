#ifndef COLISOES_H
#define COLISOES_H

#include <SDL2/SDL.h>

#include "aritmetica.h"
#include "logica.h"

typedef struct poligono {
	int n;
	SDL_FPoint centro;
	SDL_FPoint vertices[];
} poligono;

int antesDaReta(SDL_FPoint p, SDL_FPoint r1, SDL_FPoint r2);
int checarColisaoPntPlg(SDL_FPoint pnt, poligono plg, double angulo);

#endif
