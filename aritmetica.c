#include <SDL2/SDL.h>

#include <math.h>

#include "aritmetica.h"
#include <SDL2_gfxPrimitives.h>

int numeroDentroIntervalo(int n, int a, int b) {
	return (n >= a && n <= b)?1:0;
}
						
double graus(double radianos) {
	return radianos*180/M_PI;
}

double radianos(double angulo) {
	return angulo*M_PI/180;
}

double limitarDouble(double n, double limite) {
	if (n > 0)
		return n-(int)(n/limite)*limite;
	else if (n < 0)
		return n-(int)(n/limite)*limite+limite;
	return n;
}

double distanciaEntrePontos(SDL_FPoint p1, SDL_FPoint p2) {
	double a = p1.x-p2.x, b = p1.y-p2.y;
	return sqrt(a*a+b*b);
}

double anguloEntrePontos(SDL_FPoint p1, SDL_FPoint p2) {
	double angulo;
	if (p1.x-p2.x != 0) {
		angulo = graus(atan((p1.y-p2.y)/(p1.x-p2.x)));
		if (p1.x < p2.x)
			angulo += 180;
	}
	return limitarDouble(angulo,360);
}

SDL_FPoint somar(SDL_FPoint p1, SDL_FPoint p2) {
	SDL_FPoint soma = {p1.x+p2.x, p1.y+p2.y}; 
	return soma;
}

SDL_FPoint subtrair(SDL_FPoint p1, SDL_FPoint p2){
	SDL_FPoint sub = {p1.x-p2.x, p1.y-p2.y};
	return sub;
}

SDL_FPoint escalonar(SDL_FPoint p1, double escala) {
	SDL_FPoint produto = {p1.x*escala,p1.y*escala};
	return produto;
}

SDL_FPoint mover(double velocidade, double angulo) {
	SDL_FPoint newp = {velocidade*cos(radianos(angulo)), velocidade*sin(radianos(angulo))};
	return newp;
}

SDL_FPoint rotacionar(SDL_FPoint o1, SDL_FPoint p1, double angulo) {
	SDL_FPoint newp = {(p1.x-o1.x)*cos(radianos(angulo)) - (p1.y-o1.y)*sin(radianos(angulo)) + o1.x,
	                  (p1.x-o1.x)*sin(radianos(angulo)) + (p1.y-o1.y)*cos(radianos(angulo)) + o1.y};
	return newp;
}

