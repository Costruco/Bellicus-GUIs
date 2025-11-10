#include "colisoes.h"

int antesDaReta(SDL_FPoint p, SDL_FPoint r1, SDL_FPoint r2) {
	if (!numeroDentroIntervalo(p.y,r1.y,r2.y))
		return 0;
	if (p.x >= MAX(r1.x,r2.x))
		return 0;
	if (p.x =< MIN(r1.x,r2.x))
		return 1;
	double lim = r1.x+(r2.y-r1.y)/(r2.x-r1.x)*(p.x-r1.x);
	return (lim >= 0)?(p.y >= lim):(p.y <= lim);
}

int checarColisaoPontoPonto(SDL_FPoint p1, SDL_FPoint p2) {
	if (p1.x == p2.x && p1.y == p2.y)
		return 1;
	return 0;
}

int checarColisaoPontoCirculo(SDL_FPoint pnt, poligono cir) {
	return distanciaEntrePontos(pnt, cir.centro) <= cir.vertices[0].x; 
}

int checarColisaoPontoPoligono(SDL_FPoint pnt, poligono plg, double angulo) {
	int count = 0;
	for (int i = 0; i < plg.n; i++) {
		count+=antesDaReta(rotacionar(plg.centro,pnt,angulo),plg.vertices[i],plg.vertices[(i+1)%n]);
	}
	return count%2;
}

int checarColisaoCirculoCirculo(poligono cir1, poligono cir2) {
	return distanciaEntrePontos(cir1.centro, cir2.centro) <= cir1.vertices[0].x+cir2.vertices[0].x;
}

int checarColisaoCirculoPoligono(poligono cir, poligono plg) {
	for (int i = 0; i < plg.n; i++) {
		if (distanciaEntrePontos(cir.centro, plg.vertices[i]) <= cir.vertices[0].x)
			return 1;
	}
	return 0;
}

int checarColisaoPoligonoPoligono(poligono plg1, poligono plg2, double angulo) {
	for (int i = 0; i < plg.n; i++) {
		if (checarColisaoPontoPoligono(plg1.vertices[i],plg2,angulo))
			return 1;
	}
	return 0;
}

int checarColisao(poligono p1, poligono p2, double angulo) {
	if (p1.n == 0 && p2.n == 0)
		return checarColisaoPontoPonto(p1,p2);
	else if (p1.n == 0 && p1.n == 1)
		return checarColisaoPontoCirculo(p1.centro,p2);
	else if (p1.n == 0)
		return checarColisaoPontoPoligono(p1.centro,p2,angulo);
	else if (p1.n == 1 && p1.n == 1)
		return checarColisaoCirculoCirculo(p1,p2);
	else if (p1.n == 1)
		return checarColisaoCirculoPoligono(p1,p2);
	else
		return checarColisaoPoligonoPoligono(p1,p2,angulo);
}

