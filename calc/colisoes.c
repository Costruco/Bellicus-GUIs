#include "colisoes.h"

/*extern int MWIDTH,MHEIGHT;
void renderizarPoligono(SDL_Renderer * ren, poligono p, SDL_FPoint local, double angulo, int r, int g, int b, int a) {
	for (int i = 0; i < p.n; i++) {
		SDL_FPoint p1 = rotacionar(somar(p.centro,local),somar(p.vertices[i],local),angulo),
				   p2 = rotacionar(somar(p.centro,local),somar(p.vertices[(i+1)%p.n],local),angulo);
		p1 = somar(p1,(SDL_FPoint){MWIDTH,MHEIGHT});
		p2 = somar(p2,(SDL_FPoint){MWIDTH,MHEIGHT});
		lineRGBA(ren,p1.x,p1.y,
					 p2.x,p2.y,
					 r,g,b,a);
	}
}*/

int antesDaReta(SDL_FPoint p, SDL_FPoint r1, SDL_FPoint r2) {
	if (!numeroDentroIntervalo(p.y,r1.y,r2.y))
		return 0;
	return p.x < r1.x+(p.y-r1.y)/(r2.y-r1.y)*(r2.x-r1.x);
}

int checarColisaoPontoPonto(SDL_FPoint p1, SDL_FPoint p2) {
	if (aproxIgual(p1.x,p2.x) && aproxIgual(p1.y,p2.y))
		return 1;
	return 0;
}

int checarColisaoPontoCirculo(SDL_FPoint pnt, poligono cir, SDL_FPoint local2) {
	return distanciaEntrePontos(pnt, somar(cir.centro,local2)) <= cir.vertices[0].x; 
}

int checarColisaoPontoPoligono(SDL_FPoint pnt, poligono plg, SDL_FPoint local2, double angulo) {
	int count = 0;
	for (int i = 0; i < plg.n; i++) {
		count+=antesDaReta(rotacionar(somar(plg.centro,local2),pnt,angulo),somar(plg.vertices[i],local2),somar(plg.vertices[(i+1)%plg.n],local2));
	}
	return count%2;
}

int checarColisaoCirculoCirculo(poligono cir1, poligono cir2, SDL_FPoint local1, SDL_FPoint local2) {
	return distanciaEntrePontos(somar(cir1.centro,local1), somar(cir2.centro,local2)) <= cir1.vertices[0].x+cir2.vertices[0].x;
}

int checarColisaoCirculoPoligono(poligono cir, poligono plg, SDL_FPoint local1, SDL_FPoint local2, double angulo) {
	if (checarColisaoPontoPoligono(somar(cir.centro,local1),plg,local2,angulo))
		return 1;
	for (int i = 0; i < plg.n; i++) {
		if (distanciaEntrePontos(pontoProx(rotacionar(somar(plg.centro,local2),somar(cir.centro,local1),angulo),
								 		   somar(plg.vertices[i],local2),
								 		   somar(plg.vertices[(i+1)%plg.n],local2)),rotacionar(somar(plg.centro,local2),somar(cir.centro,local1),angulo)) <= cir.vertices[0].x)
			return 1;
			
	}
	return 0;
}

int checarColisaoPoligonoPoligono(poligono plg1, poligono plg2, SDL_FPoint local1, SDL_FPoint local2, double angulo) {
	for (int i = 0; i < plg1.n; i++) {
		if (checarColisaoPontoPoligono(somar(plg1.vertices[i],local1),plg2,local2,angulo))
			return 1;
	}
	return 0;
}

int checarColisao(poligono p1, poligono p2, SDL_FPoint local1, SDL_FPoint local2, double angulo) {
	if (p1.n == 0 && p2.n == 0) {
		return checarColisaoPontoPonto(somar(p1.centro,local1),somar(p2.centro,local2));
		
	} else if (p1.n+p2.n == 1) {
		if (p1.n == 0)
			return checarColisaoPontoCirculo(somar(p1.centro,local1),p2,local2);
		else
			return checarColisaoPontoCirculo(somar(p2.centro,local2),p1,local1);
			
	} else if (p1.n+p2.n >= 2 && p1.n*p2.n == 0) {
		if (p1.n == 0)
			return checarColisaoPontoPoligono(somar(p1.centro,local1),p2,local2,angulo);
		else
			return checarColisaoPontoPoligono(somar(p2.centro,local2),p1,local1,angulo);
			
	} else if (p1.n*p2.n == 1) {
		return checarColisaoCirculoCirculo(p1,p2,local1,local2);
		
	} else if (p1.n*p2.n < p1.n+p2.n) {
		if (p1.n == 1)
			return checarColisaoCirculoPoligono(p1,p2,local1,local2,angulo);
		else
			return checarColisaoCirculoPoligono(p2,p1,local2,local1,angulo);
			
	} else {
		return checarColisaoPoligonoPoligono(p1,p2,local1,local2,angulo);
	}
}
