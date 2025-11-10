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
int checarColisaoPontoPonto(SDL_FPoint p1, SDL_FPoint p2);
int checarColisaoPontoCirculo(SDL_FPoint pnt, poligono cir);
int checarColisaoPontoPoligono(SDL_FPoint pnt, poligono plg, double angulo);
int checarColisaoCirculoCirculo(poligono cir1, poligono cir2);
int checarColisaoCirculoPoligono(poligono cir, poligono plg);
int checarColisaoPoligonoPoligono(poligono plg1, poligono plg2, double angulo);
int checarColisao(poligono p1, poligono p2, double angulo);

#endif
