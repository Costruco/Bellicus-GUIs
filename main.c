#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include "menu.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


#define MAX(a,b) (a>b)?a:b
#define MIN(a,b) (a<b)?a:b
#define MOD(a) (a>=0)?a:-a

#define FPS 120
#define TPF 1000/FPS

// 1 metro = 32.5 pixels

enum Movimento {
	PONTO_MORTO,
	TRANSV,
	REV,
	ESQ_TRANSV,
	DIR_TRANSV,
	ESQ_REV,
	DIR_REV,
	ESQ,
	DIR	
};

typedef struct particula {
	SDL_FPoint local;
	int nascimento;
	int tempo_de_vida;
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
	int variacao;	
};

double graus(double radianos) {
	return radianos*180/M_PI;
}

double radianos(double angulo) {
	return angulo*M_PI/180;
}

double limitarDouble(double n, double limite) {
	if (n >= limite) {
		return n-limite;	
	} else if (n < 0) {
		return n+limite;
	}
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

int numeroEntreIntervalo(int n, int a, int b) {
	return (n >= a && n <= b)?1:0;
}

int numeroForaIntervalo(int n, int a, int b) {
	return (n < a || n > b)?1:0;
}

SDL_FPoint somar(SDL_FPoint p1, SDL_FPoint p2) {
	SDL_FPoint soma = {p1.x+p2.x, p1.y+p2.y}; 
	return soma;
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

void atualizarPosicaoProjetil(projetil * bl1) {
	SDL_FPoint ptdt = mover(bl1->velocidade/FPS,bl1->angulo);
	double dt = distanciaEntrePontos((SDL_FPoint){0,0},ptdt);
	if (bl1->distanciaAlvo-dt <= 0) {
		bl1->local = somar(bl1->local,mover(bl1->distanciaAlvo,bl1->angulo));
		bl1->distanciaAlvo = -1;
	} else {
		bl1->local = somar(bl1->local,ptdt);
		bl1->distanciaAlvo -= dt;
	}
} 

void renderizarProjetil(SDL_Renderer * ren, SDL_Texture * textura, projetil bl1, SDL_FPoint local, SDL_FPoint ctela, double escala) {
	double dx = (local.x-bl1.local.x)*escala, dy = (local.y-bl1.local.y)*escala;
	if (numeroForaIntervalo(dx,-ctela.x-50,ctela.x+50)) {
		return;
	}
	if (numeroForaIntervalo(dy,-ctela.y-50,ctela.y+50)) {
		return;
	}
	SDL_Rect recorte = {9*bl1.variacao,0,9,3};
	SDL_FRect base = {(bl1.local.x-4-local.x)*escala+ctela.x,(bl1.local.y-1-local.y)*escala+ctela.y,9*escala,3*escala};
	SDL_RenderCopyExF(ren,textura,&recorte,&base,bl1.angulo,NULL,SDL_FLIP_NONE);
}

void atualizarAnguloSoldado(soldado * sd1, SDL_FPoint alvo) {
	sd1->angulo = anguloEntrePontos(somar(sd1->local,(SDL_FPoint){16,16}),somar(alvo,sd1->erro_na_mira));
    sd1->angulo += (sd1->folga_de_fuga>0)?180:0;
}

void atualizarPosicaoSoldado(soldado * sd1, SDL_FPoint alvo) {
	double dt = distanciaEntrePontos(somar(sd1->local,(SDL_FPoint){16,16}),alvo);
	if (sd1->folga_de_fuga < 1 || dt < 120) {
		atualizarAnguloSoldado(sd1,alvo);
	}
    if (dt < 200+sd1->folga_de_fuga) {
		sd1->local = somar(sd1->local,mover(-2,sd1->angulo));
		if (sd1->folga_de_fuga < 1) {
			sd1->angulo += 180;
		}
		sd1->folga_de_fuga = 100;
	} else if (dt > 300) {
        sd1->local = somar(sd1->local,mover(-1,sd1->angulo));
    }
	sd1->folga_de_fuga = MAX(sd1->folga_de_fuga-5,0);
}

void corrigirColisao(soldado * sd1, soldado * sd2) {
	if (distanciaEntrePontos(sd1->local,sd2->local) <= 28) {
		double angulo = anguloEntrePontos(sd1->local,sd2->local);
		sd1->local = somar(sd1->local,mover(+2,angulo));
		sd2->local = somar(sd2->local,mover(-2,angulo));
	}
}

int checarVida(soldado batalhao[], int i, int * nSoldados, SDL_FPoint local, double angulo) {
	if (batalhao[i].vida == 0) {
		batalhao[i] = batalhao[(*nSoldados)-1];
		(*nSoldados)--;
		return 1;
	}
	SDL_FPoint normal = rotacionar(local,batalhao[i].local,-angulo);
	if (numeroForaIntervalo(normal.x-local.x,-97,97))
		return 0;
	if (numeroForaIntervalo(normal.y-local.y,-48,48))
		return 0;
	batalhao[i] = batalhao[(*nSoldados)-1];
	(*nSoldados)--;
	return 1;
}

void renderizarSoldado(SDL_Renderer * ren, SDL_Texture * textura, soldado sd1, SDL_FPoint local, SDL_FPoint ctela, double escala) {
	double dx = (local.x-sd1.local.x)*escala, dy = (local.y-sd1.local.y)*escala;
	if (numeroForaIntervalo(dx,-ctela.x-50,ctela.x+50)) {
		return;
	}
	if (numeroForaIntervalo(dy,-ctela.y-50,ctela.y+50)) {
		return;
	}
	SDL_Rect recorte = {32*sd1.variacao,0,32,32};
	SDL_FRect base = {(sd1.local.x-16-local.x)*escala+ctela.x,(sd1.local.y-16-local.y)*escala+ctela.y,32*escala,32*escala};
	SDL_RenderCopyExF(ren,textura,&recorte,&base,sd1.angulo,NULL,SDL_FLIP_NONE);
}

int main(int argc, char* args[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window * win = SDL_CreateWindow("Bellicus", 0, 0, 0, 0, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | 0x00001000);
	SDL_Renderer * ren = SDL_CreateRenderer(win, -1, 0);
	SDL_ShowCursor(SDL_DISABLE);
	srand(SDL_GetTicks());
	MenuOption escolha = menu_loop(ren, win);

	//MENU DO JOGO
	if(escolha == MENU_PLAY){
		//sprites
		SDL_Texture * chassiHD = IMG_LoadTexture(ren, "./sprites/chassi_com_sombra.png"),
					* chassiLQ = IMG_LoadTexture(ren, "./sprites/chassi_baixa_qualidade.png"),
				    * torreHD = IMG_LoadTexture(ren, "./sprites/torre_com_sombra.png"),
					* torreLQ = IMG_LoadTexture(ren, "./sprites/torre_baixa_qualidade.png"),
					* reticula1 = IMG_LoadTexture(ren, "./sprites/reticula_opaca.png"),
					* reticula2 = IMG_LoadTexture(ren, "./sprites/reticula_translucida.png"),
                    * soldados = IMG_LoadTexture(ren, "./sprites/soldados.png"),
					* municao = IMG_LoadTexture(ren, "./sprites/municao.png"),
					* explosao = IMG_LoadTexture(ren, "./sprites/explosao.png");
		
		//angulos e posições das lagartas
		double angulo = 0, 
			   angulo_arma = 0, 
			   angulo_alvo = 0;
			
		//valores do jogo
		int WIDTH, 
			HEIGHT;
		SDL_GetWindowSize(win, &WIDTH,&HEIGHT);
		int MWIDTH = WIDTH/2, MHEIGHT = HEIGHT/2;
		
		//valores do veiculo
		int VELOCIDADE_TRANS_MAXIMA = 441,
			VELOCIDADE_REV_MAXIMA = 88,
			ACELERACAO_TRANS = 10,
			ACELERACAO_REV = 7,
			DESACELERACAO = 5;
		double velocidade = 0,
			   VELOCIDADE_ANGULAR = 0.9,
			   VELOCIDADE_TORRE = 0.5,
			   TEMPO_DE_RECARGA = 1000,
			   VELOCIDADE_DE_RECARGA = 1,
			   zoom = 0.9;	
			
		SDL_FPoint torre_offset = {19,0},
				   zero = {0,0},
				   centro_tanque = {MWIDTH,MHEIGHT},
				   centro_torre_absoluto = somar(centro_tanque,torre_offset),
				   centro_torre_relativo = torre_offset,
				   mira_real,
				   local = {0,0};
				  		
		particula marcos[100];
		soldado batalhao[100];
		projetil hell[100];
		int nParticulas = 0,
			nSoldados = 0,
			nBalas = 0;
		
		const Uint8 * tecP = SDL_GetKeyboardState(NULL);
			
		//informações dos paineis
		char x[30],
			 y[30],
			 grau[30],
			 velocimetro[30],
			 numero[30],
			 state[30],
			 angulotorre[30],
			 anguloalvo[30];
			
		//estado inicial
		sprintf(state, "Estado: PONTO_MORTO");
		
		enum Movimento estado = PONTO_MORTO;
		int gamerunning = 1,
			mx=0,my=0,
			esperaPorInimigo = 2000,
			ultimoDisparo = SDL_GetTicks()-TEMPO_DE_RECARGA/VELOCIDADE_DE_RECARGA,
			ultimoSpawn = SDL_GetTicks();
			
		Uint32 espera = TPF;
		SDL_Event evt;
		while (gamerunning) {
			//recebe os inputs e atualiza o estado de movimentação do tanque
			int isevt = AUX_WaitEventTimeoutCount(&evt, &espera);
			if (isevt) {
				switch (evt.type) {	
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					if (tecP[SDL_SCANCODE_W] && !tecP[SDL_SCANCODE_S]) {
						if (tecP[SDL_SCANCODE_A]) {
							estado = ESQ_TRANSV;
							sprintf(state, "Estado: ESQ_TRANSV");
						} else if (tecP[SDL_SCANCODE_D]) {
							estado = DIR_TRANSV;
							sprintf(state, "Estado: DIR_TRANSV");
						} else {
							estado = TRANSV;
							sprintf(state, "Estado: TRANSV");
						}
							
					} else if (tecP[SDL_SCANCODE_S]  && !tecP[SDL_SCANCODE_W]) {
						if (tecP[SDL_SCANCODE_A]) {
							estado = ESQ_REV;
							sprintf(state, "Estado: ESQ_REV");
						} else if (tecP[SDL_SCANCODE_D]) {
							estado = DIR_REV;
							sprintf(state, "Estado: DIR_REV");
						} else {
							estado = REV;
							sprintf(state, "Estado: REV");
						}
							
					} else if (tecP[SDL_SCANCODE_A] && !tecP[SDL_SCANCODE_D]) {
						estado = ESQ;
						sprintf(state, "Estado: ESQ");
						
						
					} else if (tecP[SDL_SCANCODE_D] && !tecP[SDL_SCANCODE_A]) {
						estado = DIR;
						sprintf(state, "Estado: DIR");
						
						
					} else {
						estado = PONTO_MORTO;
						sprintf(state, "Estado: PONTO_MORTO");
					} 
					
					
					if (tecP[SDL_SCANCODE_KP_PLUS] && zoom < 3.0) {
						zoom += 0.05;
					} else if (tecP[SDL_SCANCODE_KP_MINUS] && zoom > 0.1) {
						zoom -= 0.05;
					}
					break;
				case SDL_QUIT:
					gamerunning = 0;
					break;
				case SDL_MOUSEMOTION:
					SDL_GetMouseState(&mx,&my);
					break;
				case SDL_MOUSEBUTTONDOWN:
					//spawn de projeteis
					int b1;
					if (evt.button.button == SDL_BUTTON_LEFT && nBalas < 100 && SDL_GetTicks()-ultimoDisparo >= TEMPO_DE_RECARGA/VELOCIDADE_DE_RECARGA) {
						ultimoDisparo = SDL_GetTicks();
						SDL_FPoint ponta_do_canhao = somar(somar(local,rotacionar(zero,torre_offset,angulo)),mover(100,angulo_arma));
						projetil newbl = {ponta_do_canhao,
										  distanciaEntrePontos(ponta_do_canhao,somar(local,escalonar((SDL_FPoint){mira_real.x-MWIDTH,mira_real.y-MHEIGHT},1/zoom))),
										  0,
										  angulo_arma,
										  1000,
										  1};
						hell[nBalas++] = newbl;
					}
					break;
				}
			}
			//atualiza o frame
			if (espera == 0) {
				espera = TPF;
				//fundo verde
				SDL_SetRenderDrawColor(ren,0,100,0,255);
				SDL_RenderClear(ren);
				
				//grade chao
				int m;
				for (m = limitarDouble(local.x*zoom,100*zoom); m < WIDTH+100; m+=100*zoom) {
					lineRGBA(ren, WIDTH-m,-100,WIDTH-m,HEIGHT+100,0,50,0,255);
				}
				for (m = limitarDouble(local.y*zoom,100*zoom); m < HEIGHT+100; m+=100*zoom) {
					lineRGBA(ren, -100,HEIGHT-m,WIDTH+100,HEIGHT-m,0,50,0,255);
				}
				
				//centro da torre
				centro_torre_relativo = rotacionar(zero,escalonar(torre_offset,zoom),angulo);
				centro_torre_absoluto = somar(centro_torre_relativo,centro_tanque);
				SDL_FPoint base = somar(escalonar((SDL_FPoint){-37,-34},zoom),centro_torre_absoluto);
				SDL_FPoint centro_torre = escalonar((SDL_FPoint){37,34},zoom);
				
				//encontra o angulo da mira
				SDL_FPoint mouse = {mx,my};	
				angulo_alvo = anguloEntrePontos(mouse,centro_torre_absoluto);
			
				//ponto onde o canhao realmente esta mirando
				double distancia = hypot(mx-centro_torre_absoluto.x,my-centro_torre_absoluto.y);
				mira_real = somar(centro_torre_absoluto,mover(distancia,angulo_arma));
				
				//spawn e atualiza��o de soldados
				int s1,s2;
                if (nSoldados < 96 && SDL_GetTicks()-ultimoSpawn >= esperaPorInimigo) {
                	ultimoSpawn = SDL_GetTicks();
                	SDL_FPoint coord;
                	if (rand()%2 == 0) {
                		//coordenada fora dos limites superior e inferior da tela
                		coord = {rand()%(WIDTH+201)-MWIDTH-100,rand()%2*(HEIGHT+201)-MHEIGHT-100};
					} else {
						coord = {rand()%2*(WIDTH+201)-MWIDTH-100,rand()%(HEIGHT+201)-MHEIGHT-100};
						//coordenada fora dos limites esquerdo e direito da tela
					}
					coord = somar(coord,local);
                	for (s1 = 0; s1 < 5; s1++) {
	                	SDL_FPoint var = {rand()%65,rand()%65};
	                	soldado newsd = {somar(coord,var),
	                					 1,
	                					 rand()%4,
										 0,
										 0,
										 (SDL_FPoint){SDL_GetTicks()%15,SDL_GetTicks()%15}};
						batalhao[nSoldados++] = newsd;
					}
				}
				for (s1 = 0; s1 < nSoldados; s1++) {
					if (checarVida(batalhao,s1,&nSoldados,local,angulo))
						continue;
					atualizarPosicaoSoldado(&batalhao[s1],local);
					for (s2 = s1+1; s2 < nSoldados; s2++) {
						corrigirColisao(&batalhao[s1],&batalhao[s2]);
					}
					renderizarSoldado(ren,soldados,batalhao[s1],local,centro_tanque,zoom);
				}
                
				//chassi do tanque
				SDL_Texture * chassi;
				if (zoom >= 0.2)
					chassi = chassiHD;
				else 
					chassi = chassiLQ;
				SDL_FRect base_chassi = {MWIDTH-101*zoom,MHEIGHT-52*zoom,203*zoom,104*zoom};
				SDL_RenderCopyExF(ren,chassi,NULL,&base_chassi,angulo,NULL,SDL_FLIP_NONE);
				
				//atualiza��o de projeteis
				int b1;
				for (b1 = 0; b1 < nBalas; b1++) {
					if (hell[b1].distanciaAlvo <= 0) {
						for (s1 = 0; s1 < nSoldados; s1++) {
							if (numeroForaIntervalo(batalhao[s1].local.x,hell[b1].local.x-50,hell[b1].local.x+50))
								continue;
							if (numeroForaIntervalo(batalhao[s1].local.y,hell[b1].local.y-50,hell[b1].local.y+50))
								continue;
							batalhao[s1].vida = 0;
						}
						if (nParticulas < 100) {
							particula newpart = {(SDL_FPoint){hell[b1].local.x-50,hell[b1].local.y-50},SDL_GetTicks(),500};
							marcos[nParticulas++] = newpart;
						}
						hell[b1] = hell[nBalas---1];
						continue;
					}
					atualizarPosicaoProjetil(&hell[b1]);
					renderizarProjetil(ren,municao,hell[b1],local,centro_tanque,zoom);
				}
				
				//laser desejado/laser real
				lineRGBA(ren,centro_torre_absoluto.x,centro_torre_absoluto.y,
				 		 mira_real.x-16*cos(radianos(angulo_arma)),
						 mira_real.y-16*sin(radianos(angulo_arma)),
						 255,255,255,25);
				
				//torre do tanque
				SDL_Texture * torre;
				if (zoom >= 0.2)
					torre = torreHD;
				else 
					torre = torreLQ;
				SDL_FRect base_torre = {base.x,base.y,146*zoom,68*zoom};
				SDL_RenderCopyExF(ren, torre, NULL, &base_torre, angulo_arma, &centro_torre, SDL_FLIP_NONE);
				
				//atualiza��o de particulas
				int p1;
				for (p1 = 0; p1 < nParticulas; p1++) {
					int tempo_vivo = SDL_GetTicks()-marcos[p1].nascimento;
					if (marcos[p1].tempo_de_vida < tempo_vivo) {
						marcos[p1] = marcos[nParticulas---1];
					}
					SDL_Rect recorte = {100*(int)(tempo_vivo/100),0,100,100};
					SDL_FRect base_explosao = {(marcos[p1].local.x-local.x)*zoom+MWIDTH,(marcos[p1].local.y-local.y)*zoom+MHEIGHT,100*zoom,100*zoom};
					SDL_RenderCopyExF(ren,explosao,&recorte,&base_explosao,0,NULL,SDL_FLIP_NONE);
				}
				
				//reticula desejada/reticula real
				SDL_FRect r1 = {mx-20,my-20,41,41};
				SDL_RenderCopyExF(ren,reticula1,NULL,&r1,0,NULL,SDL_FLIP_NONE);
				
				SDL_FRect r2 = {mira_real.x-20,mira_real.y-20,41,41};
				SDL_RenderCopyExF(ren,reticula2,NULL,&r2,0,NULL,SDL_FLIP_NONE);
                
				//painel de controle do chassi
				sprintf(x, "x: %4.1lf", local.x);
				sprintf(y, "y: %4.1lf", -local.y);
				sprintf(grau, "Angulo: %3.2lf", angulo);
				sprintf(velocimetro, "Velocidade: ");
				sprintf(numero, "%2.1lf", velocidade*1.2/10);
				boxRGBA(ren,0,0,169,57,0,0,0,255);
				stringRGBA(ren, 3, 3, x, 255,255,255,255);
				stringRGBA(ren, 3, 14, y, 255,255,255,255);
				stringRGBA(ren, 3, 25, grau, 255,255,255,255);
				stringRGBA(ren, 3, 36, velocimetro, 255,255,255,255);
				stringRGBA(ren, 100, 36, numero, 255,250-0.50*(MOD(velocidade)),250-0.50*(MOD(velocidade)),255);
				stringRGBA(ren, 3, 47, state, 255,255,255,255);
				rectangleRGBA(ren, 0,0,170,58,255,255,255,255);
				
				//painel de controle da torre
				sprintf(angulotorre, "angulo da torre: %4.1lf", angulo_arma);
				sprintf(anguloalvo, "angulo alvo: %4.1lf", angulo_alvo);
				boxRGBA(ren,0,HEIGHT-25,179,HEIGHT,0,0,0,255);
				stringRGBA(ren, 3, HEIGHT-23, angulotorre, 255,255,255,255);
				stringRGBA(ren, 3, HEIGHT-12, anguloalvo, 255,255,255,255);
				rectangleRGBA(ren, 0,HEIGHT-26,180,HEIGHT,255,255,255,255);
				
				//atualiza a velocidade e direção do movimento do tanque com base no estado
				if (estado == PONTO_MORTO) {
					;		
				} else if (estado == TRANSV) {
					if (velocidade < VELOCIDADE_TRANS_MAXIMA-ACELERACAO_TRANS)
						velocidade += ACELERACAO_TRANS;
					else
						velocidade = VELOCIDADE_TRANS_MAXIMA;						
				} else if (estado == ESQ_TRANSV) {
					if (velocidade < VELOCIDADE_TRANS_MAXIMA-ACELERACAO_TRANS)
						velocidade += ACELERACAO_TRANS;
					else
						velocidade = VELOCIDADE_TRANS_MAXIMA;
					angulo -= VELOCIDADE_ANGULAR;
					angulo_arma -= VELOCIDADE_ANGULAR;
				} else if (estado == DIR_TRANSV) {
					if (velocidade < VELOCIDADE_TRANS_MAXIMA-ACELERACAO_TRANS)
						velocidade += ACELERACAO_TRANS;
					else
						velocidade = VELOCIDADE_TRANS_MAXIMA;
					angulo += VELOCIDADE_ANGULAR;
					angulo_arma += VELOCIDADE_ANGULAR;		
				} else if (estado == REV) {
					if (velocidade > -VELOCIDADE_REV_MAXIMA+ACELERACAO_REV)
						velocidade -= ACELERACAO_REV;
					else
						velocidade = -VELOCIDADE_REV_MAXIMA;
				} else if (estado == ESQ_REV) {
					if (velocidade > -VELOCIDADE_REV_MAXIMA+ACELERACAO_REV)
						velocidade -= ACELERACAO_REV;
					else
						velocidade = -VELOCIDADE_REV_MAXIMA;
					angulo += VELOCIDADE_ANGULAR;
					angulo_arma += VELOCIDADE_ANGULAR;		
				} else if (estado == DIR_REV) {
					if (velocidade > -VELOCIDADE_REV_MAXIMA+ACELERACAO_REV)
						velocidade -= ACELERACAO_REV;
					else
						velocidade = -VELOCIDADE_REV_MAXIMA;
					angulo -= VELOCIDADE_ANGULAR;
					angulo_arma -= VELOCIDADE_ANGULAR;
				} else if (estado == ESQ) {
					angulo -= VELOCIDADE_ANGULAR;
					angulo_arma -= VELOCIDADE_ANGULAR;
				} else if (estado == DIR) {
					angulo += VELOCIDADE_ANGULAR;
					angulo_arma += VELOCIDADE_ANGULAR;			
				}
				//atualiza os angulos
				angulo_arma = limitarDouble(angulo_arma,360);
				angulo = limitarDouble(angulo,360);
					
				if (angulo_arma == angulo_alvo)
					;
				else if (angulo_arma+VELOCIDADE_TORRE > angulo_alvo && angulo_arma-VELOCIDADE_TORRE < angulo_alvo) {
					angulo_arma = angulo_alvo;
				} else {
					if (angulo_arma >= 180 && (((int)angulo_arma+180)%360 > angulo_alvo || angulo_arma < angulo_alvo))
						angulo_arma += VELOCIDADE_TORRE;
					else if (((int)angulo_arma+180)%360 > angulo_alvo && angulo_arma < angulo_alvo)
						angulo_arma += VELOCIDADE_TORRE;
					else
						angulo_arma -= VELOCIDADE_TORRE;
				}
				//atualiza a posição do tanque e do ponteiro
				SDL_FPoint deslocamento = mover(velocidade/FPS,angulo);
				local = somar(local, deslocamento);
				
				//atualiza a velocidade do tanque
				if (velocidade > 0)
					velocidade = MAX(velocidade-DESACELERACAO, 0);
				else
					velocidade = MIN(velocidade+DESACELERACAO, 0);
				//nao exige explicações
				SDL_RenderPresent(ren);
			}
		}
	} else if(escolha == MENU_TUTORIAL){
		printf("Tutorial iniciado!");
	} else if(escolha == MENU_CREDITS){
		printf("Bruno, Eraldo e Marcos");
	}
	
	//finaliza a aplicação
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
