#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <SDL2/SDL.h>

typedef struct poligono {
	int n;
	SDL_FPoint centro;
	SDL_FPoint * vertices;
} poligono;

typedef struct terreno {
	SDL_FPoint local;
    double angulo;
    double velocidade;
} terreno;

typedef struct particula {
	SDL_FPoint local;
	int nascimento;
	int tempo_de_vida;
	int tipo;
	int var;
} particula;

typedef struct soldado {
	SDL_FPoint local;
	int vida;
	int variacao;
	double folga_de_fuga;
	double angulo;	
	SDL_FPoint erro_na_mira;
} soldado;

typedef struct projetil {
	SDL_FPoint local;
	double distanciaAlvo;
	double peso;
	double angulo;
	double velocidade;
	int tipo;
	int variacao;	
} projetil;

#endif

