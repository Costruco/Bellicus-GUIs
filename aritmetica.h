#ifndef ARITMETICA_H
#define ARITMETICA_H

#include <SDL2/SDL.h>

#define MAX(a,b) (a>b)?a:b
#define MIN(a,b) (a<b)?a:b
#define MOD(a) (a>=0)?a:-a

int numeroDentroIntervalo(double n, double a, double b);
int anguloDentroIntervalo(double n, double a, double b);
double graus(double radianos);
double radianos(double angulo);
double limitarDouble(double n, double limite);
int limitarInt(int n, int limite);
double distanciaEntrePontos(SDL_FPoint p1, SDL_FPoint p2);
double anguloEntrePontos(SDL_FPoint p1, SDL_FPoint p2);
SDL_FPoint somar(SDL_FPoint p1, SDL_FPoint p2);
SDL_FPoint escalonar(SDL_FPoint p1, double escala);
SDL_FPoint mover(double velocidade, double angulo);
SDL_FPoint rotacionar(SDL_FPoint o1, SDL_FPoint p1, double angulo);

#endif


