#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include "menu.h"
#include <math.h>
#include <stdio.h>


#define MAX(a,b) (a>b)?a:b
#define MIN(a,b) (a<b)?a:b
#define MOD(a) (a>=0)?a:-a

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

int main(int argc, char* args[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window * win = SDL_CreateWindow("Bellicus", 0, 0, 0, 0, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | 0x00001000);
	SDL_Renderer * ren = SDL_CreateRenderer(win, -1, 0);
	SDL_ShowCursor(SDL_DISABLE);

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
                    * soldado1 = IMG_LoadTexture(ren, "./sprites/soldado.png");
		
		//angulos e posições das lagartas
		double angulo = 0, 
			angulo_arma = 0, 
			angulo_alvo = 0;
			
		//valores do jogo
		int WIDTH, 
			HEIGHT, 
			FPS = 60, 
			TPF = 1000/FPS;
		SDL_GetWindowSize(win, &WIDTH,&HEIGHT);
		
		//valores do veiculo
		int VELOCIDADE_TRANS_MAXIMA = 441,
			VELOCIDADE_REV_MAXIMA = 88,
			ACELERACAO_TRANS = 10,
			ACELERACAO_REV = 7,
			DESACELERACAO = 5;
		double velocidade = 0,
			   VELOCIDADE_ANGULAR = 0.9,
			   VELOCIDADE_TORRE = 0.5,
			   zoom = 0.9,
			   mx=0,my=0;	
			
		SDL_FPoint torre_offset = {19,0},
						zero = {0,0},
				  		centro_tanque = {WIDTH/2,HEIGHT/2},
						centro_soldado1 = {216,216},
						centro_soldado2 = {316,316},
				  		local = {0,0};
		double folga_de_fuga1 = 0,folga_de_fuga2 = 0;
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
		    intmx=0,intmy=0;
		Uint32 espera = TPF;
		SDL_Event evt;
		while (gamerunning) {
			//recebe os inputs e atualiza o estado de movimentação do tanque
			int isevt = AUX_WaitEventTimeoutCount(&evt, &espera);
			if (isevt) {	
				if (evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP) {
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
				} else if (evt.type == SDL_QUIT) {
					gamerunning = 0;
				} else if (evt.type == SDL_MOUSEMOTION) {
					SDL_GetMouseState(&intmx,&intmy);
					mx = intmx, my = intmy;
				}
			}
			
			//atualiza o angulo do chassi
			angulo = limitarDouble(angulo,360);
			
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
				SDL_FPoint centro_torre_relativo = rotacionar(zero,escalonar(torre_offset,zoom),angulo);
				SDL_FPoint centro_torre_absoluto = somar(centro_torre_relativo,centro_tanque);
				SDL_FPoint base = somar(escalonar((SDL_FPoint){-37,-34},zoom),centro_torre_absoluto);
				SDL_FPoint centro_torre = escalonar((SDL_FPoint){37,34},zoom);
				
				//encontra o angulo da mira
				SDL_FPoint mouse = {mx,my};	
				angulo_alvo = anguloEntrePontos(mouse,centro_torre_absoluto);
			
				//ponto onde o canhao realmente esta mirando
				double distancia = hypot(mx-centro_torre_absoluto.x,my-centro_torre_absoluto.y);
				SDL_FPoint mira_real = somar(centro_torre_absoluto,mover(distancia,angulo_arma));
				
				
				//Soldado teste arrumar o angulo
                double angulosd1 = anguloEntrePontos((SDL_FPoint){(centro_soldado1.x-local.x)*zoom+WIDTH/2,(centro_soldado1.y-local.y)*zoom+HEIGHT/2},centro_tanque);
                double angulosd2 = anguloEntrePontos((SDL_FPoint){(centro_soldado2.x-local.x)*zoom+WIDTH/2,(centro_soldado2.y-local.y)*zoom+HEIGHT/2},centro_tanque);
                SDL_FRect sd1 = {(centro_soldado1.x-16-local.x)*zoom+WIDTH/2,(centro_soldado1.y-16-local.y)*zoom+HEIGHT/2,32*zoom,32*zoom};
                SDL_FRect sd2 = {(centro_soldado2.x-16-local.x)*zoom+WIDTH/2,(centro_soldado2.y-16-local.y)*zoom+HEIGHT/2,32*zoom,32*zoom};
                
                angulosd1 += (folga_de_fuga1>0)?180:0;
                angulosd2 += (folga_de_fuga2>0)?180:0;
                if (distanciaEntrePontos(centro_soldado1,local) < 200+folga_de_fuga1) {
	        		centro_soldado1 = somar(centro_soldado1,mover(-5,angulosd1));
	        		folga_de_fuga1 = 100;
	        	} else if (distanciaEntrePontos(centro_soldado1,local) > 300) {
	                centro_soldado1 = somar(centro_soldado1,mover(-3,angulosd1));
                }
	        	folga_de_fuga1 = MAX(folga_de_fuga1-5,0);
                if (distanciaEntrePontos(centro_soldado2,local) < 200+folga_de_fuga2) {
	        		centro_soldado2 = somar(centro_soldado2,mover(-5,angulosd2));	
	        		folga_de_fuga2 = 100;
	        	} else if (distanciaEntrePontos(centro_soldado2,local) > 300) {
	                centro_soldado2 = somar(centro_soldado2,mover(-3,angulosd2));
	        	}
	        	folga_de_fuga2 = MAX(folga_de_fuga2-5,0);
            	if (distanciaEntrePontos(centro_soldado1,centro_soldado2) <= 20) {
            		if (angulosd2 > angulosd1) {
            			centro_soldado2 = somar(centro_soldado2,mover(-2,angulosd2-90));
	                	centro_soldado1 = somar(centro_soldado1,mover(-2,angulosd1+90));
	            	} else {
	            		centro_soldado2 = somar(centro_soldado2,mover(-2,angulosd2+90));
	                	centro_soldado1 = somar(centro_soldado1,mover(-2,angulosd1-90));
					}
	        	}
	        	SDL_RenderCopyExF(ren,soldado1,NULL,&sd2,angulosd2,NULL,SDL_FLIP_NONE);
                SDL_RenderCopyExF(ren,soldado1,NULL,&sd1,angulosd1,NULL,SDL_FLIP_NONE);
                
                
				//chassi do tanque
				SDL_Texture * chassi;
				if (zoom >= 0.2)
					chassi = chassiHD;
				else 
					chassi = chassiLQ;
				SDL_FRect base_chassi = {WIDTH/2-101*zoom,HEIGHT/2-52*zoom,203*zoom,104*zoom};
				SDL_RenderCopyExF(ren,chassi,NULL,&base_chassi,angulo,NULL,SDL_FLIP_NONE);
				
				//laser desejado/laser real
				lineRGBA(ren,centro_torre_absoluto.x,centro_torre_absoluto.y,
				 		 mira_real.x-16*cos(radianos(angulo_arma)),
						 mira_real.y-16*sin(radianos(angulo_arma)),
						 255,255,255,50);
				
				//torre do tanque
				SDL_Texture * torre;
				if (zoom >= 0.2)
					torre = torreHD;
				else 
					torre = torreLQ;
				SDL_FRect base_torre = {base.x,base.y,146*zoom,68*zoom};
				SDL_RenderCopyExF(ren, torre, NULL, &base_torre, angulo_arma, &centro_torre, SDL_FLIP_NONE);
				
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
				//atualiza o angulo da arma
				angulo_arma = limitarDouble(angulo_arma,360);
					
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
