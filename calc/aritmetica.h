#ifndef ARITMETICA_H
#define ARITMETICA_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <math.h>

#define MOD(a) (a>=0)?a:-a

int intPow(int a, int b);
void swap(int * a, int * b);
void swapF(float * a, float * b);
int truncArr(double n, int nCasas);
double graus(double radianos);
double radianos(double angulo);
double limitarDouble(double n, double limite);
int limitarInt(int n, int limite);
double distanciaEntrePontos(SDL_FPoint p1, SDL_FPoint p2);
double anguloEntrePontos(SDL_FPoint p1, SDL_FPoint p2);
double produtoEscalar(SDL_FPoint p1, SDL_FPoint p2);
SDL_FPoint somar(SDL_FPoint p1, SDL_FPoint p2);
SDL_FPoint sub(SDL_FPoint p1, SDL_FPoint p2);
SDL_FPoint escalonar(SDL_FPoint p1, double escala);
SDL_FPoint mover(double velocidade, double angulo);
SDL_FPoint rotacionar(SDL_FPoint o1, SDL_FPoint p1, double angulo);
SDL_FPoint pontoProx(SDL_FPoint p, SDL_FPoint a, SDL_FPoint b);

#endif


