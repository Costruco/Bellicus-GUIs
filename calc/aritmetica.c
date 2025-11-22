#include "aritmetica.h"
#include "logica.h"

int intPow(int a, int b) {
	int pot = 1;
	while (b-- != 0) {
		pot*=a;
	}
	return pot;
}

void swap(int * a, int * b) {
	int aux = *a;
	*a = *b;
	*b = aux;
}


void swapF(float * a, float * b) {
	float aux = *a;
	*a = *b;
	*b = aux;
}

int truncArr(double n, int nCasas) {
	int teto = intPow(10,nCasas), resto = limitarInt(n,teto);
	if (resto > teto/2)
		return (int)n-resto+teto;
	else if (resto < teto/2)
		return (int)n-resto;
	else {
		if ((((int)n)/teto)%2 == 0)
			return (int)n-resto+teto;
		else
			return (int)n-resto;
	}		
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

int limitarInt(int n, int limite) {
	if (n > 0)
		return n-n/limite*limite;
	else if (n < 0)
		return n-n/limite*limite+limite;
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

double produtoEscalar(SDL_FPoint p1, SDL_FPoint p2) {
	return p1.x*p2.x+p1.y*p2.y;
}

SDL_FPoint somar(SDL_FPoint p1, SDL_FPoint p2) {
	return (SDL_FPoint){p1.x+p2.x, p1.y+p2.y};
}

SDL_FPoint sub(SDL_FPoint p1, SDL_FPoint p2){
	return (SDL_FPoint){p1.x-p2.x, p1.y-p2.y};
}

SDL_FPoint escalonar(SDL_FPoint p1, double escala) {
	return (SDL_FPoint){p1.x*escala,p1.y*escala};
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

SDL_FPoint pontoProx(SDL_FPoint p, SDL_FPoint a, SDL_FPoint b) {
	SDL_FPoint v = sub(b,a), u = sub(a,p);
	double t = -produtoEscalar(v,u)/produtoEscalar(v,v);
	if (numeroDentroIntervalo(t,0,1))
		return somar(escalonar(a,1-t),escalonar(b,t));
	else if (distanciaEntrePontos(p,a) < distanciaEntrePontos(p,b))
		return a;
	else
		return b;
}

