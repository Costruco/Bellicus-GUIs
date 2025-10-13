#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "aritmetica.h"
#include "espera.h"
#include "estados.h"
#include "interface.h"
#include "menu.h"

// 1 metro = 32.5 pixels

//o FPS � GLOBAL!
int FPS = 120, TPF = 8;

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
	if (!numeroDentroIntervalo(dx,-ctela.x-50,ctela.x+50)) {
		return;
	}
	if (!numeroDentroIntervalo(dy,-ctela.y-50,ctela.y+50)) {
		return;
	}
	SDL_Rect recorte;
	SDL_FRect base;
	
	if (bl1.tipo == 0 ){
		recorte = (SDL_Rect){9*bl1.variacao,0,9,2};
		base = (SDL_FRect){(bl1.local.x-4.5-local.x)*escala+ctela.x,(bl1.local.y-1-local.y)*escala+ctela.y,9*escala,2*escala};
	} else if(bl1.tipo==1){
		recorte = (SDL_Rect){50*bl1.variacao,0,50,1};
		base = (SDL_FRect){(bl1.local.x-25-local.x)*escala+ctela.x,(bl1.local.y-0.5-local.y)*escala+ctela.y,50*escala,1*escala};
	}
    else{
            recorte = (SDL_Rect){0,0,4,4};
		    base = (SDL_FRect){(bl1.local.x-2-local.x)*escala+ctela.x,(bl1.local.y-2-local.y)*escala+ctela.y,4*escala,4*escala};    
    }
	
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
		sd1->local = somar(sd1->local,mover(-1,sd1->angulo));
		if (sd1->folga_de_fuga < 1) {
			sd1->angulo += 180;
		}
		sd1->folga_de_fuga = 100;
	} else if (dt > 300) {
        sd1->local = somar(sd1->local,mover(-0.5,sd1->angulo));
    }
	sd1->folga_de_fuga = MAX(sd1->folga_de_fuga-2.5,0);
}

void corrigirColisao(soldado * sd1, soldado * sd2) {
	if (distanciaEntrePontos(sd1->local,sd2->local) <= 28) {
		double angulo = anguloEntrePontos(sd1->local,sd2->local);
		sd1->local = somar(sd1->local,mover(+2,angulo));
		sd2->local = somar(sd2->local,mover(-2,angulo));
	}
}

int checarVida(soldado batalhao[], int i, int * nSoldados, SDL_FPoint local, double angulo, terreno sangue[], double velocidade, int * nSangue) {
	if (batalhao[i].vida == 0) {
		batalhao[i] = batalhao[--(*nSoldados)];
		return 1;
	}
	SDL_FPoint normal = rotacionar(local,batalhao[i].local,-angulo);
	if (!numeroDentroIntervalo(normal.x-local.x,-97,97))
		return 0;
	if (!numeroDentroIntervalo(normal.y-local.y,-48,48))
		return 0;
    
	terreno newsangue = {(SDL_FPoint){batalhao[i].local.x,batalhao[i].local.y},
					     angulo+180,
						 velocidade};
	sangue[((*nSangue)++)%100] = newsangue;
    
	batalhao[i] = batalhao[--(*nSoldados)];
	return 1;
}

int colisaoBala(soldado batalhao[], terreno sangue[], int i, projetil bullet, double angulo, int* nSoldados, int* nSangue){
	if (distanciaEntrePontos(batalhao[i].local, bullet.local) < 16){
		terreno newsangue = {(SDL_FPoint){batalhao[i].local.x, batalhao[i].local.y}, angulo+180,0};

		sangue[((*nSangue)++)%100] = newsangue;

		batalhao[i] = batalhao[--(*nSoldados)];
		return 1;
	}
	return 0;
	
}

void renderizarSoldado(SDL_Renderer * ren, SDL_Texture * textura, soldado sd1, SDL_FPoint local, SDL_FPoint ctela, double escala) {
	double dx = (local.x-sd1.local.x)*escala, dy = (local.y-sd1.local.y)*escala;
	if (!numeroDentroIntervalo(dx,-ctela.x-50,ctela.x+50)) {
		return;
	}
	if (!numeroDentroIntervalo(dy,-ctela.y-50,ctela.y+50)) {
		return;
	}
	SDL_Rect recorte = {32*sd1.variacao,0,32,32};
	SDL_FRect base = {(sd1.local.x-16-local.x)*escala+ctela.x,(sd1.local.y-16-local.y)*escala+ctela.y,32*escala,32*escala};
	SDL_RenderCopyExF(ren,textura,&recorte,&base,sd1.angulo,NULL,SDL_FLIP_NONE);
}

int main(int argc, char* args[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window * win = SDL_CreateWindow("Bellicus", 0, 0, 0, 0, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | 0x00001000);
	int WIDTH, 
		HEIGHT;
	int MAP_X_SIZE = 10000,
		MAP_Y_SIZE = 10000;
	SDL_GetWindowSize(win, &WIDTH,&HEIGHT);
	int MWIDTH = WIDTH/2, MHEIGHT = HEIGHT/2;
	SDL_Renderer * ren = SDL_CreateRenderer(win, -1, 0);
	SDL_ShowCursor(SDL_DISABLE);
	srand(SDL_GetTicks());
	int apprunning = 1;
	
	while (!SDL_QuitRequested() && apprunning) {
		MenuOption escolha = menu_loop(ren, win, WIDTH, HEIGHT);
		//MENU DO JOGO
		if(escolha == MENU_PLAY){
			//sprites
			SDL_Texture * chassiHD = IMG_LoadTexture(ren, "./sprites/chassi_com_sombra.png"),
						* chassiLQ = IMG_LoadTexture(ren, "./sprites/chassi_baixa_qualidade.png"),
					    * torreHD = IMG_LoadTexture(ren, "./sprites/torre_com_sombra.png"),
						* torreLQ = IMG_LoadTexture(ren, "./sprites/torre_baixa_qualidade.png"),
						* cupola = IMG_LoadTexture(ren, "./sprites/cupola_metralhadora_chassi.png"),
						* metralhadora_chassi = IMG_LoadTexture(ren, "./sprites/metralhadora_chassi.png"),
						* reticula1 = IMG_LoadTexture(ren, "./sprites/reticula_opaca.png"),
						* reticula2 = IMG_LoadTexture(ren, "./sprites/reticula_translucida.png"),
	                    * soldados = IMG_LoadTexture(ren, "./sprites/soldados.png"),
						* municao = IMG_LoadTexture(ren, "./sprites/municao.png"),
						* municao_metralhadora = IMG_LoadTexture(ren, "./sprites/municao_metralhadora.png"),
						* explosao = IMG_LoadTexture(ren, "./sprites/explosao.png"),
						* cratera = IMG_LoadTexture(ren, "./sprites/cratera.png"),
	                    * sangue_arrasto = IMG_LoadTexture(ren, "./sprites/sangue_arrasto.png"),
						* flash1 = IMG_LoadTexture(ren, "./sprites/flash_metralhadora_chassi.png"),
						* flash2 = IMG_LoadTexture(ren, "./sprites/flash_cupola_chassi.png"),
						* flash3 = IMG_LoadTexture(ren, "./sprites/flash_metralhadora_coaxial.png"),
						* fumaca = IMG_LoadTexture(ren, "./sprites/fumaca.png"),
	                    * municao_soldado = IMG_LoadTexture(ren, "./sprites/balaSoldado.png"),
						* tile_map = IMG_LoadTexture(ren, "./sprites/tile_map.png");
			
			//angulos e posições das lagartas
			double angulo = 0, 
				   angulo_arma = 0,
				   angulo_metra = 0, 
				   angulo_alvo = 0;
			
			//cores
			SDL_Color branco = {255,255,255,255};
			
			//valores do veiculo
			int VELOCIDADE_TRANS_MAXIMA = 441,
				VELOCIDADE_REV_MAXIMA = 88,
				ACELERACAO_TRANS = 600,
				ACELERACAO_REV = 420,
				DESACELERACAO = 300;
			double velocidade = 0,
				   VELOCIDADE_ANGULAR = 54,
				   VELOCIDADE_TORRE = 30,
				   TEMPO_DE_RECARGA = 1000,
				   TEMPO_DE_RECARGA_METRA = 100,
				   VELOCIDADE_DE_RECARGA = 1,
				   VELOCIDADE_DE_RECARGA_METRA = 1,
				   zoom = 0.9;	
				
			SDL_FPoint torre_offset = {19,0},
					   metra_coaxial_offset = {42,7},
					   metra_chassi_offset = {65,17},
					   cupola_offset = {62,17},
					   exaustor1_offset = {-93,-15},
					   exaustor2_offset = {-93,+15},
					   centro_tanque = {MWIDTH,MHEIGHT},
					   centro_torre = {MWIDTH+5,MHEIGHT},
					   zero = {0,0},
					   centro_torre_absoluto = somar(centro_tanque,torre_offset),
					   centro_torre_relativo = torre_offset,
					   mira_real,
					   local = {0.1,0.1};
			terreno sangue[100];		  		
			terreno obstaculos[100];
			particula marcos[300];
			soldado batalhao[100];
			projetil hell[100],
					 bullet[100],
	                 bala[100];
			int nObstaculos = 0,
				nParticulas = 0,
				nSoldados = 0,
				nBalas = 0,
				nBalasMetra = 0,
	            nBalasSoldado=0,
	            nSangue=0;
			
			const Uint8 * tecP = SDL_GetKeyboardState(NULL);
			
			movimento estado = PONTO_MORTO;
			int seed = rand(),
				gamerunning = 1,
				debug = 0,
				grid = 0,
				mx=0,my=0,
				esperaPorInimigo = 2000,
				esperaPorFumaca = 7,
				ultimoDisparo = SDL_GetTicks()-TEMPO_DE_RECARGA/VELOCIDADE_DE_RECARGA,
				ultimoDisparoMetraChassi = SDL_GetTicks()-TEMPO_DE_RECARGA_METRA/VELOCIDADE_DE_RECARGA_METRA,
				ultimoDisparoMetraCoaxial = SDL_GetTicks()-TEMPO_DE_RECARGA_METRA/VELOCIDADE_DE_RECARGA_METRA,
	            ultimoDisparoSoldado = SDL_GetTicks()-TEMPO_DE_RECARGA/VELOCIDADE_DE_RECARGA,
				ultimoSpawn = SDL_GetTicks(),
				ultimaFumaca = SDL_GetTicks();
			
			Uint32 espera = TPF;
			SDL_Event evt;
			
			SDL_Point mapa[MAP_X_SIZE/100][MAP_Y_SIZE/100];
			for (int i = 0; i < MAP_X_SIZE/100; i++) {
				for (int j = 0; j < MAP_Y_SIZE/100; j++) {
					mapa[i][j].x = rand()%16;
					mapa[i][j].y = rand()%3;
				}
			}
			//medidor de FPS
			int ticksPerFrame[10], nTicks = 0, ultimoFrame = SDL_GetTicks();
			
			while (gamerunning && apprunning) {
				//recebe os inputs e atualiza o estado de movimentação do tanque
				int isevt = AUX_WaitEventTimeoutCount(&evt, &espera);
				if (isevt) {
					int esq_nao_dir = (tecP[SDL_SCANCODE_A] && !tecP[SDL_SCANCODE_D]),
						dir_nao_esq = (!tecP[SDL_SCANCODE_A] && tecP[SDL_SCANCODE_D]);
					switch (evt.type) {	
					case SDL_KEYDOWN:
					case SDL_KEYUP:
						if (tecP[SDL_SCANCODE_W] && !tecP[SDL_SCANCODE_S]) {
							if (esq_nao_dir)
								estado = ESQ_TRANSV;
							else if (dir_nao_esq)
								estado = DIR_TRANSV;
							else
								estado = TRANSV;
								
						} else if (tecP[SDL_SCANCODE_S] && !tecP[SDL_SCANCODE_W]) {
							if (esq_nao_dir)
								estado = ESQ_REV;
							else if (dir_nao_esq)
								estado = DIR_REV;
							else
								estado = REV;
								
						} else if (esq_nao_dir)
							estado = ESQ;	
								
						else if (dir_nao_esq)
							estado = DIR;	
							
						else
							estado = PONTO_MORTO;
						
						if (tecP[SDL_SCANCODE_KP_PLUS] && tecP[SDL_SCANCODE_KP_MINUS])
							;
						else if (tecP[SDL_SCANCODE_KP_MINUS] && zoom > 0.1)
							zoom -= 0.05;
						else if (tecP[SDL_SCANCODE_KP_PLUS] && zoom < 3.0)
							zoom += 0.05;
							
						if (tecP[SDL_SCANCODE_F3]) {
							if (debug == 0)
								debug = 1;
							else
								debug = 0;
						}
						if (tecP[SDL_SCANCODE_G]) {
							if (grid == 0)
								grid = 1;
							else
								grid = 0;
						}
						if (tecP[SDL_SCANCODE_ESCAPE]) {
							gamerunning = 0;
						}
						if (tecP[SDL_SCANCODE_SPACE]) {
							//spawn de projetil de metralhadora
							if (SDL_GetTicks()-ultimoDisparoMetraCoaxial >= TEMPO_DE_RECARGA_METRA/VELOCIDADE_DE_RECARGA_METRA && nBalasMetra < 100) {
								ultimoDisparoMetraCoaxial = SDL_GetTicks();
								SDL_FPoint ponta_da_metra_coaxial = somar(somar(local,
																				rotacionar(zero,torre_offset,angulo)),
																		  rotacionar(zero,metra_coaxial_offset,angulo_arma));
								projetil newtiro = {ponta_da_metra_coaxial,
													3000,
													0,
													angulo_arma-2+rand()%3,
													2000,
													1,
													0};
								bullet[nBalasMetra++] = newtiro;
								if (nParticulas < 300) {
									particula newpart = {zero,
													     SDL_GetTicks(),
													     16,
													     4,
														 0};
									marcos[nParticulas++] = newpart;
								}
							}
						}
						break;
					case SDL_QUIT:
						apprunning = 0;
						break;
					case SDL_MOUSEMOTION:
						SDL_GetMouseState(&mx,&my);
						break;
					case SDL_MOUSEBUTTONDOWN:
						//spawn de projeteis
						if (evt.button.button == SDL_BUTTON_LEFT && nBalas < 100 && SDL_GetTicks()-ultimoDisparo >= TEMPO_DE_RECARGA/VELOCIDADE_DE_RECARGA) {
							ultimoDisparo = SDL_GetTicks();
							SDL_FPoint ponta_do_canhao = somar(somar(local,rotacionar(zero,torre_offset,angulo)),mover(100,angulo_arma));
							projetil newbl = {ponta_do_canhao,
											  distanciaEntrePontos(ponta_do_canhao,somar(local,escalonar((SDL_FPoint){mira_real.x-MWIDTH,mira_real.y-MHEIGHT},1/zoom))),
											  0,
											  angulo_arma,
											  3000,
											  0,
											  1};
							hell[nBalas++] = newbl;
						}
						break;
					}
				}
				//atualiza o frame
				if (espera == 0) {
					espera = TPF;
					
					//atualiza o FPS
					ticksPerFrame[nTicks%10] = SDL_GetTicks()-ultimoFrame;
					ultimoFrame = SDL_GetTicks();
					nTicks++;
					int soma = 0;
					for (int i = 0; i < ((nTicks>10)?10:nTicks); i++) {
						soma += ticksPerFrame[i];
					}
					FPS = 1000*((nTicks>10)?10:nTicks)/((soma!=0)?soma:1);
					
					//limpa a tela
					SDL_RenderClear(ren);
					
					//particula dos exaustores
					SDL_FPoint centro_ex1_relativo = rotacionar(zero,exaustor1_offset,angulo);
					SDL_FPoint centro_ex1_absoluto = somar(centro_ex1_relativo,local);
					
					SDL_FPoint centro_ex2_relativo = rotacionar(zero,exaustor2_offset,angulo);
					SDL_FPoint centro_ex2_absoluto = somar(centro_ex2_relativo,local);
					
					SDL_FPoint centro_exaustores = escalonar((SDL_FPoint){12.5,12.5},zoom);
					
					if (SDL_GetTicks()-ultimaFumaca > esperaPorFumaca && nParticulas < 299) {
						ultimaFumaca = SDL_GetTicks();
						SDL_FPoint vento = mover(rand()%13,angulo+180-10+rand()%21);
						particula newpart = {somar(centro_ex1_absoluto,(SDL_FPoint){vento.x,vento.y}),
											 SDL_GetTicks(),
											 200,
											 3,
											 0};
						marcos[nParticulas++] = newpart;
						vento = (SDL_FPoint) mover(rand()%13,angulo+180-10+rand()%21);
						newpart = (particula){somar(centro_ex2_absoluto,(SDL_FPoint){vento.x,vento.y}),
								   SDL_GetTicks(),
								   200,
								   3,
								   0};
						marcos[nParticulas++] = newpart;		 
					}
	
					//centro da torre
					centro_torre_relativo = rotacionar(zero,escalonar(torre_offset,zoom),angulo);
					centro_torre_absoluto = somar(centro_torre_relativo,centro_tanque);
					SDL_FPoint base = somar(escalonar((SDL_FPoint){-37,-34},zoom),centro_torre_absoluto);
					centro_torre = escalonar((SDL_FPoint){37,34},zoom);
					
					//centro da metralhadora do chassi
					SDL_FPoint centro_metra_relativo = rotacionar(zero,escalonar(metra_chassi_offset,zoom),angulo);
					SDL_FPoint centro_metra_absoluto = somar(centro_tanque,centro_metra_relativo);
					SDL_FPoint base_metra = somar(escalonar((SDL_FPoint){-3,-3},zoom),centro_metra_absoluto);
					SDL_FPoint centro_metra = escalonar((SDL_FPoint){3,3},zoom);
					
					//centro da cupola do chassi
					SDL_FPoint centro_cupola_relativo = rotacionar(zero,escalonar(cupola_offset,zoom),angulo);
					SDL_FPoint centro_cupola_absoluto = somar(centro_tanque,centro_cupola_relativo);
					SDL_FPoint base_cup = somar(escalonar((SDL_FPoint){-5,-5},zoom),centro_cupola_absoluto);
					SDL_FPoint centro_cupola = escalonar((SDL_FPoint){5,5},zoom);
					
					//centro do flash da metralhadora 
					SDL_FPoint centro_flash_relativo = rotacionar(zero,escalonar(metra_chassi_offset,zoom),angulo);
					SDL_FPoint centro_flash_absoluto = somar(centro_tanque,centro_flash_relativo);
					SDL_FPoint base_flash;
					SDL_FPoint centro_flash;
					
					//centro do flash da metralhadora coaxial
					SDL_FPoint centro_flash_1_relativo = somar(rotacionar(zero,escalonar(torre_offset,zoom),angulo),
															   rotacionar(zero,escalonar(metra_coaxial_offset,zoom),angulo_arma));
					SDL_FPoint centro_flash_1_absoluto = somar(centro_tanque,centro_flash_1_relativo);
					SDL_FPoint base_flash_1 = somar(escalonar((SDL_FPoint){-16,-16},zoom),centro_flash_1_absoluto);;
					SDL_FPoint centro_flash_1 = escalonar((SDL_FPoint){16,16},zoom);;
					
					//metralhadora movel
					SDL_FPoint ponta_da_metra_chassi;
					if(SDL_GetTicks()-ultimoDisparoMetraChassi >= TEMPO_DE_RECARGA_METRA/VELOCIDADE_DE_RECARGA_METRA){
						ponta_da_metra_chassi = somar(somar(local, rotacionar(zero, metra_chassi_offset, angulo)), mover(14, angulo_metra));
						
						//centro do flash da metralhadora 
						base_flash = somar(escalonar((SDL_FPoint){-5,-15},zoom),centro_flash_absoluto);
						centro_flash = escalonar((SDL_FPoint){5,15},zoom);
						
						for (int s = 0; s < nSoldados; s++) {
							if (nBalasMetra < 100) {
								double anguloDeDisparo = anguloEntrePontos(batalhao[s].local,ponta_da_metra_chassi);
								int noCampoDeVisao = anguloDentroIntervalo(anguloDeDisparo,angulo-45,angulo+45),
									noAlcance = distanciaEntrePontos(batalhao[s].local,ponta_da_metra_chassi) < distanciaEntrePontos(zero,(SDL_FPoint){MWIDTH+50,MHEIGHT+50});
								if (noCampoDeVisao && noAlcance) {
									ultimoDisparoMetraChassi = SDL_GetTicks();
									angulo_metra = anguloDeDisparo;
									projetil newtiro = {ponta_da_metra_chassi,
														3000,
														0,
														angulo_metra-3+rand()%7,
														2000,
														1,
														0};
									bullet[nBalasMetra++] = newtiro;
									if (nParticulas < 299) {
										particula newpart = {base_flash,
														     SDL_GetTicks(),
														     16,
														     1,
															 0};
										marcos[nParticulas++] = newpart;
										newpart = (particula) {base_cup,
															   SDL_GetTicks(),
															   16,
															   2,
															   0};
										marcos[nParticulas++] = newpart;
									}
									break;
								}
							}
						}
					}
					
	                //tiros soldados
	                if(SDL_GetTicks()-ultimoDisparoSoldado >= TEMPO_DE_RECARGA/VELOCIDADE_DE_RECARGA) {
	                	ultimoDisparoSoldado = SDL_GetTicks();
		                for (int s = 0; s < nSoldados; s++) {
		                	if (nBalasSoldado < 100) {
			                    double anguloBalaSoldado = anguloEntrePontos(local,batalhao[s].local);
			                    if(distanciaEntrePontos(batalhao[s].local,local) < 400 && batalhao[s].folga_de_fuga <= 0) {
			                        projetil balanova = {batalhao[s].local,
														 distanciaEntrePontos(batalhao[s].local,local),
														 0,
														 anguloBalaSoldado-8+rand()%17,
														 200,
														 2,
														 0};
			                        bala[nBalasSoldado++] = balanova;
			                    }
			            	} else 
			            		break;
						}
	            	}
	            	
	            	//textura do chao
	            	int inicio_x = -MWIDTH/zoom-100-limitarInt(-MWIDTH/zoom+(int)local.x,100),
	            		inicio_y = -MHEIGHT/zoom-100-limitarInt(-MHEIGHT/zoom+(int)local.y,100),
						fim_x = MWIDTH/zoom+100,
						fim_y = MHEIGHT/zoom+100;
						
					int xneg = (inicio_x+local.x < -MAP_X_SIZE/2),
						xpos = (fim_x+local.x > MAP_X_SIZE/2),
						yneg = (inicio_y+local.y < -MAP_Y_SIZE/2),
						ypos = (fim_y+local.y > MAP_Y_SIZE/2);
						
	            	if (xneg)
	            		inicio_x = -MAP_X_SIZE/2-local.x;
	            	if (xpos)
	            		fim_x = MAP_X_SIZE/2-local.x;
	            	if (yneg)
	            		inicio_y = -MAP_Y_SIZE/2-local.y;
	            	if (ypos)
	            		fim_y = MAP_Y_SIZE/2-local.y;
	            		
	            	for (int m = inicio_x, i = (m+(int)local.x+MAP_X_SIZE/2)/100; m < fim_x; m+=100, i++) {
						for (int n = inicio_y, j = (n+(int)local.y+MAP_Y_SIZE/2)/100; n < fim_y; n+=100, j++) {
							SDL_Rect recorte = {(mapa[i][j].x%4)*100,
												(mapa[i][j].x/4)*100,
												100,100};
							SDL_FRect base = {m*zoom+MWIDTH,n*zoom+MHEIGHT,100*zoom,100*zoom};
							SDL_RenderCopyExF(ren,tile_map,&recorte,&base,0,NULL,(SDL_RendererFlip)mapa[i][j].y);
							
							//coordenada dos pontos da grade
							if (debug && grid) {
								circleRGBA(ren,m*zoom+MWIDTH,n*zoom+MHEIGHT,5,0,0,255,255);
								char string[30];
								sprintf(string,"%d,%d",m+(int)local.x,n+(int)local.y);
								stringRGBA(ren,m*zoom+MWIDTH+5,n*zoom+MHEIGHT+5,string,255,255,255,255);
								sprintf(string,"%d",m+n+(int)local.x+(int)local.y);
								stringRGBA(ren,m*zoom+MWIDTH+5,n*zoom+MHEIGHT+16,string,255,255,255,255);
							}
						}
					}
					if (xneg)
	            		thickLineRGBA(ren,inicio_x*zoom+MWIDTH,inicio_y*zoom+MHEIGHT,inicio_x*zoom+MWIDTH,fim_y*zoom+MHEIGHT,3,255,0,0,255);
	            	if (xpos)
	            		thickLineRGBA(ren,fim_x*zoom+MWIDTH,inicio_y*zoom+MHEIGHT,fim_x*zoom+MWIDTH,fim_y*zoom+MHEIGHT,3,255,0,0,255);
	            	if (yneg)
	            		thickLineRGBA(ren,inicio_x*zoom+MWIDTH,inicio_y*zoom+MHEIGHT,fim_x*zoom+MWIDTH,inicio_y*zoom+MHEIGHT,3,255,0,0,255);
	            	if (ypos)
	            		thickLineRGBA(ren,inicio_x*zoom+MWIDTH,fim_y*zoom+MHEIGHT,fim_x*zoom+MWIDTH,fim_y*zoom+MHEIGHT,3,255,0,0,255);
					
					//grade chao
					if (grid) {
						for (int m = -MWIDTH/zoom+100-limitarInt(-MWIDTH/zoom+(int)local.x,100); m < MWIDTH/zoom; m+=100) {
							lineRGBA(ren,m*zoom+MWIDTH,0,m*zoom+MWIDTH,HEIGHT,0,50,0,255);
						}
						for (int n = -MHEIGHT/zoom+100-limitarInt(-MHEIGHT/zoom+(int)local.y,100); n < MHEIGHT/zoom; n+=100) {
							lineRGBA(ren,0,n*zoom+MHEIGHT,WIDTH,n*zoom+MHEIGHT,0,50,0,255);
						}
					}
					
					//encontra o angulo da mira
					SDL_FPoint mouse = {mx,my};	
					angulo_alvo = anguloEntrePontos(mouse,centro_torre_absoluto);
				
					//ponto onde o canhao realmente esta mirando
					double distancia = hypot(mx-centro_torre_absoluto.x,my-centro_torre_absoluto.y);
					mira_real = somar(centro_torre_absoluto,mover(distancia,angulo_arma));
					
					//atualização do terreno;
					for (int t1 = 0; t1 < nObstaculos && t1<100; t1++) {
						SDL_Rect recorte = {0,0,100,100};
						SDL_FRect base_obs = {(obstaculos[t1].local.x-local.x-50)*zoom+MWIDTH,(obstaculos[t1].local.y-local.y-50)*zoom+MHEIGHT,100*zoom,100*zoom};
						SDL_RenderCopyExF(ren,cratera,&recorte,&base_obs,0,NULL,SDL_FLIP_NONE);
					}
					for (int t1 = 0; t1 < nObstaculos && t1<100; t1++) {
						SDL_Rect recorte = {100,0,100,100};
						SDL_FRect base_obs = {(obstaculos[t1].local.x-local.x-50)*zoom+MWIDTH,(obstaculos[t1].local.y-local.y-50)*zoom+MHEIGHT,100*zoom,100*zoom};
						SDL_RenderCopyExF(ren,cratera,&recorte,&base_obs,0,NULL,SDL_FLIP_NONE);
					}
					
	                //sangue
	                for (int t1 = 0; t1 < nSangue && t1<100; t1++) {
						SDL_Rect recorte = {0,0,60,50};
						SDL_FRect base_sangue = {(sangue[t1].local.x-local.x-30)*zoom+MWIDTH,
												 (sangue[t1].local.y-local.y-25)*zoom+MHEIGHT,
												 ((sangue[t1].velocidade>200)?sangue[t1].velocidade/200:1)*60*zoom,
												 50*zoom};
						SDL_RenderCopyExF(ren,sangue_arrasto,&recorte,&base_sangue,sangue[t1].angulo,NULL,SDL_FLIP_NONE);
					}
					for (int t1 = 0; t1 < nSangue && t1<100; t1++) {
						SDL_Rect recorte = {60,0,60,50};
						SDL_FRect base_sangue = {(sangue[t1].local.x-local.x-30)*zoom+MWIDTH,
												 (sangue[t1].local.y-local.y-25)*zoom+MHEIGHT,
												 ((sangue[t1].velocidade>200)?sangue[t1].velocidade/200:1)*60*zoom,
												 50*zoom};
						SDL_RenderCopyExF(ren,sangue_arrasto,&recorte,&base_sangue,sangue[t1].angulo,NULL,SDL_FLIP_NONE);
					}
					
					//spawn e atualização de soldados
					int s1,s2;
	                if (nSoldados < 96 && SDL_GetTicks()-ultimoSpawn >= esperaPorInimigo) {
	                	ultimoSpawn = SDL_GetTicks();
	                	SDL_FPoint coord;
	                	if (rand()%2 == 0) {
	                		//coordenada fora dos limites superior e inferior da tela
	                		coord = (SDL_FPoint){rand()%(WIDTH+201)-MWIDTH-100,rand()%2*(HEIGHT+201)-MHEIGHT-100};
						} else {
							coord = (SDL_FPoint){rand()%2*(WIDTH+201)-MWIDTH-100,rand()%(HEIGHT+201)-MHEIGHT-100};
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
											 (SDL_FPoint){rand()%15,rand()%15}};
							batalhao[nSoldados++] = newsd;
						}
					}
					for (s1 = 0; s1 < nSoldados; s1++) {
						if (checarVida(batalhao,s1,&nSoldados,local,angulo,sangue,velocidade,&nSangue)) {
							continue;
						}
						atualizarPosicaoSoldado(&batalhao[s1],local);
						for (s2 = s1+1; s2 < nSoldados; s2++) {
							corrigirColisao(&batalhao[s1],&batalhao[s2]);
						}
						renderizarSoldado(ren,soldados,batalhao[s1],local,centro_tanque,zoom);
					}
	                
					//chassi do tanque
					SDL_Texture * chassi;
					if (zoom >= 0.2)
						chassi = chassiHD; //eu sou o eraldo dou muito a bunda 
					else 
						chassi = chassiLQ;
					SDL_FRect base_chassi = {MWIDTH-101*zoom,MHEIGHT-52*zoom,203*zoom,104*zoom};
					SDL_RenderCopyExF(ren,chassi,NULL,&base_chassi,angulo,NULL,SDL_FLIP_NONE);
					
					//metralhadora do chassi
					SDL_FRect base_metralhadora = {base_metra.x,base_metra.y,14*zoom,8*zoom};
					SDL_RenderCopyExF(ren,metralhadora_chassi,NULL,&base_metralhadora,angulo_metra,&centro_metra,SDL_FLIP_NONE);
					
					//flash da metralhadora
					for (int p1 = 0; p1 < nParticulas; p1++) {
						int tempo_vivo = SDL_GetTicks()-marcos[p1].nascimento;
						if (marcos[p1].tempo_de_vida <= tempo_vivo) {
							marcos[p1] = marcos[--nParticulas];
							p1--;
						}
						SDL_FRect base;
						switch (marcos[p1].tipo) {
							case 1:
								base = (SDL_FRect){base_flash.x,base_flash.y,37*zoom,33*zoom};
								SDL_RenderCopyExF(ren,flash1,NULL,&base,angulo_metra,&centro_flash,SDL_FLIP_NONE);
								break;
						}
					}
					
					//cupola da metralhadora do chassi
					SDL_FRect base_cupola = {base_cup.x,base_cup.y,11*zoom,12*zoom};
					SDL_RenderCopyExF(ren,cupola,NULL,&base_cupola,angulo,&centro_cupola,SDL_FLIP_NONE);
					
					//flash da cupola
					for (int p1 = 0; p1 < nParticulas; p1++) {
						int tempo_vivo = SDL_GetTicks()-marcos[p1].nascimento;
						if (marcos[p1].tempo_de_vida < tempo_vivo) {
							marcos[p1] = marcos[--nParticulas];
							p1--;
						}
						SDL_FRect base;
						switch (marcos[p1].tipo) {
							case 2:
								base = (SDL_FRect){base_cup.x,base_cup.y,11*zoom,12*zoom};
								SDL_RenderCopyExF(ren,flash2,NULL,&base,angulo,&centro_cupola,SDL_FLIP_NONE);
								break;
						}
					}
					
					//atualização de projeteis
					int b1;
					for (b1 = 0; b1 < nBalas; b1++) {
						atualizarPosicaoProjetil(&hell[b1]);
						renderizarProjetil(ren,municao,hell[b1],local,centro_tanque,zoom);
						if (hell[b1].distanciaAlvo <= 0) {
							if (nParticulas < 300) {
								particula newpart = {(SDL_FPoint){hell[b1].local.x,hell[b1].local.y},
													 SDL_GetTicks(),
													 1000,
													 0,
													 0};
								marcos[nParticulas++] = newpart;
							}
							
							terreno newobs = {(SDL_FPoint){hell[b1].local.x,hell[b1].local.y},0,0};
							obstaculos[(nObstaculos++)%100] = newobs;
							
							hell[b1] = hell[--nBalas];
							b1--;
						}
					}
					int b2, s;
					for(b2 = 0; b2 < nBalasMetra; b2++){
						atualizarPosicaoProjetil(&bullet[b2]);
						renderizarProjetil(ren,municao_metralhadora, bullet[b2], local, centro_tanque, zoom);
						int bala_fora = (bullet[b2].distanciaAlvo <= 0),
							matou = 0;
						for(s = 0; s < nSoldados; s++){
							if((colisaoBala(batalhao, sangue, s, bullet[b2], bullet[b2].angulo, &nSoldados, &nSangue))){
								matou = 1;
								break;
							}
						}
						if((matou || bala_fora) && bullet[b2].tipo == 1){
							bullet[b2] = bullet[--nBalasMetra];
	        				b2--;
						}
					}
					
	                int b3;
	                for(b3 = 0; b3 < nBalasSoldado; b3++){
	                	atualizarPosicaoProjetil(&bala[b3]);
	                	renderizarProjetil(ren,municao_soldado, bala[b3], local, centro_tanque, zoom);
	                	int bala_fora = (bala[b3].distanciaAlvo <= 0);
						if(bala_fora && bala[b3].tipo == 2){
							bala[b3] = bala[--nBalasSoldado];
	        				b3--;
						}
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
					
					//atualização de particulas
					for (int p1 = 0; p1 < nParticulas; p1++) {
						int tempo_vivo = SDL_GetTicks()-marcos[p1].nascimento;
						if (marcos[p1].tempo_de_vida <= tempo_vivo) {
							marcos[p1] = marcos[--nParticulas];
							p1--;
						}
						int estagio;
						SDL_FPoint vento;
						SDL_Rect recorte;
						SDL_FRect base;
						switch (marcos[p1].tipo) {
							case 0:
								for (s1 = 0; s1 < nSoldados; s1++) {
									if (!numeroDentroIntervalo(batalhao[s1].local.x,hell[b1].local.x-7.5*tempo_vivo/100,hell[b1].local.x+7.5*tempo_vivo/100))
										continue;
									if (!numeroDentroIntervalo(batalhao[s1].local.y,hell[b1].local.y-7.5*tempo_vivo/100,hell[b1].local.y+7.5*tempo_vivo/100))
										continue;
									batalhao[s1].vida = 0;
								}
								recorte = (SDL_Rect){100*(int)((tempo_vivo%200)/40),100*(int)(tempo_vivo/200),100,100};
								base = (SDL_FRect){(marcos[p1].local.x-local.x-100)*zoom+MWIDTH,(marcos[p1].local.y-local.y-100)*zoom+MHEIGHT,200*zoom,200*zoom};
								SDL_RenderCopyExF(ren,explosao,&recorte,&base,0,NULL,SDL_FLIP_NONE);
								break;
							case 3:
								estagio = (int)(tempo_vivo/25);
								vento = mover(1.5,angulo+180);
								recorte = (SDL_Rect){50*estagio,0,50,50};
								base = (SDL_FRect){(marcos[p1].local.x-local.x-12.5)*zoom+MWIDTH+zoom*vento.x*(int)(tempo_vivo/10),
										(marcos[p1].local.y-local.y-12.5)*zoom+MHEIGHT+zoom*vento.y*(int)(tempo_vivo/10),
										25*zoom,
										25*zoom};
								SDL_RenderCopyExF(ren,fumaca,&recorte,&base,rand()%360,&centro_exaustores,SDL_FLIP_NONE);
								break;
							case 4:
								base = (SDL_FRect){base_flash_1.x,base_flash_1.y,33*zoom,33*zoom};
								SDL_RenderCopyExF(ren,flash3,NULL,&base,angulo_arma,&centro_flash_1,SDL_FLIP_NONE);
								break;
								
						}
					}
					
					//reticula desejada/reticula real
					SDL_FRect r1 = {mx-20,my-20,41,41};
					SDL_RenderCopyExF(ren,reticula1,NULL,&r1,0,NULL,SDL_FLIP_NONE);
					
					SDL_FRect r2 = {mira_real.x-20,mira_real.y-20,41,41};
					SDL_RenderCopyExF(ren,reticula2,NULL,&r2,0,NULL,SDL_FLIP_NONE);
	                
					//DEBUG
					if (debug) {
						char * debuggers1[] = {"N Soldados:     %5.0lf",
											   "N Particulas:   %5.0lf",
											   "N BalasMetra:   %5.0lf",
											   "N BalasSoldado: %5.0lf",
											   "N Sangue:       %5.0lf",
											   "FPS:            %5.0lf",
											   ""};
						double debugData1[] = {nSoldados,
											  nParticulas,
											  nBalasMetra,
											  nBalasSoldado,
											  nSangue,
											  FPS};
						doubleDataLabel(ren,WIDTH-21*8-6,0,7,debuggers1,debugData1,NULL);
						
						char estadoString[12];
						stateToString(estadoString,estado);
						stringDataLabel(ren,WIDTH-21*8-6,0,6,"Estado: %s",estadoString,NULL);
						
						char string[12];
						dataBox(ren,MWIDTH-11*4-3,0,11,1);
						sprintf(string,"%d %d %d %d %d %d",tecP[SDL_SCANCODE_W],tecP[SDL_SCANCODE_A],tecP[SDL_SCANCODE_S],tecP[SDL_SCANCODE_D],tecP[SDL_SCANCODE_KP_PLUS],tecP[SDL_SCANCODE_KP_MINUS]);
						stringRGBA(ren,MWIDTH-11*4,3,string,255,255,255,255);
					}
					
					//painel de controle do chassi
					char * controleChassi[] = {"x: %14.0lf",
											   "y: %14.0lf",
											   "Angulo: %9.2lf",
											   "Velocidade: -",
											   "            %5.1lf"};
					double infoChassi[] = {local.x,
										   -local.y,
										   angulo,
										   0,
										   velocidade*1.2/10};
					SDL_Color coresChassi[] = {branco,
											   branco,
											   branco,
											   branco,
											   (SDL_Color){255,250-0.50*(MOD(velocidade)),250-0.50*(MOD(velocidade)),255}};
					doubleDataLabel(ren,0,0,5,controleChassi,infoChassi,coresChassi);
					
					//painel de controle da torre
					char * controleTorre[] = {"angulo da torre: %6.2lf",
											  "angulo alvo:     %6.2lf"};
					double infoTorre[] = {angulo_arma,
										  angulo_alvo};
					doubleDataLabel(ren,0,HEIGHT-25,2,controleTorre,infoTorre,NULL);		
					
					//atualiza a velocidade e direção do movimento do tanque com base no estado
					if (estado == PONTO_MORTO) {
						;		
					} else if (estado == TRANSV) {
						if (velocidade < VELOCIDADE_TRANS_MAXIMA-ACELERACAO_TRANS/FPS)
							velocidade += ACELERACAO_TRANS/FPS;
						else
							velocidade = VELOCIDADE_TRANS_MAXIMA;						
					} else if (estado == ESQ_TRANSV) {
						if (velocidade < VELOCIDADE_TRANS_MAXIMA-ACELERACAO_TRANS/FPS)
							velocidade += ACELERACAO_TRANS/FPS;
						else
							velocidade = VELOCIDADE_TRANS_MAXIMA;
						angulo -= VELOCIDADE_ANGULAR/FPS;
						angulo_arma -= VELOCIDADE_ANGULAR/FPS;
						angulo_metra -= VELOCIDADE_ANGULAR/FPS;
					} else if (estado == DIR_TRANSV) {
						if (velocidade < VELOCIDADE_TRANS_MAXIMA-ACELERACAO_TRANS/FPS)
							velocidade += ACELERACAO_TRANS/FPS;
						else
							velocidade = VELOCIDADE_TRANS_MAXIMA;
						angulo += VELOCIDADE_ANGULAR/FPS;
						angulo_arma += VELOCIDADE_ANGULAR/FPS;
						angulo_metra += VELOCIDADE_ANGULAR/FPS;	
					} else if (estado == REV) {
						if (velocidade > -VELOCIDADE_REV_MAXIMA+ACELERACAO_REV/FPS)
							velocidade -= ACELERACAO_REV/FPS;
						else
							velocidade = -VELOCIDADE_REV_MAXIMA;
					} else if (estado == ESQ_REV) {
						if (velocidade > -VELOCIDADE_REV_MAXIMA+ACELERACAO_REV/FPS)
							velocidade -= ACELERACAO_REV/FPS;
						else
							velocidade = -VELOCIDADE_REV_MAXIMA;
						angulo += VELOCIDADE_ANGULAR/FPS;
						angulo_arma += VELOCIDADE_ANGULAR/FPS;
						angulo_metra += VELOCIDADE_ANGULAR/FPS;		
					} else if (estado == DIR_REV) {
						if (velocidade > -VELOCIDADE_REV_MAXIMA+ACELERACAO_REV/FPS)
							velocidade -= ACELERACAO_REV/FPS;
						else
							velocidade = -VELOCIDADE_REV_MAXIMA;
						angulo -= VELOCIDADE_ANGULAR/FPS;
						angulo_arma -= VELOCIDADE_ANGULAR/FPS;
						angulo_metra -= VELOCIDADE_ANGULAR/FPS;
					} else if (estado == ESQ) {
						angulo -= VELOCIDADE_ANGULAR/FPS;
						angulo_arma -= VELOCIDADE_ANGULAR/FPS;
						angulo_metra -= VELOCIDADE_ANGULAR/FPS;
					} else if (estado == DIR) {
						angulo += VELOCIDADE_ANGULAR/FPS;
						angulo_arma += VELOCIDADE_ANGULAR/FPS;
						angulo_metra += VELOCIDADE_ANGULAR/FPS;
					}
					//atualiza os angulos
					angulo_arma = limitarDouble(angulo_arma,360);
					angulo = limitarDouble(angulo,360);
					angulo_metra = limitarDouble(angulo_metra,360);
						
					if (angulo_arma == angulo_alvo)
						;
					else if (angulo_arma+VELOCIDADE_TORRE/FPS > angulo_alvo && angulo_arma-VELOCIDADE_TORRE/FPS < angulo_alvo) {
						angulo_arma = angulo_alvo;
					} else {
						if (angulo_arma >= 180 && (limitarDouble(angulo_arma+180,360) > angulo_alvo || angulo_arma < angulo_alvo))
							angulo_arma += VELOCIDADE_TORRE/FPS;
						else if (limitarDouble(angulo_arma+180,360) > angulo_alvo && angulo_arma < angulo_alvo)
							angulo_arma += VELOCIDADE_TORRE/FPS;
						else
							angulo_arma -= VELOCIDADE_TORRE/FPS;
					}
					//atualiza a posição do tanque e do ponteiro
					SDL_FPoint deslocamento = mover(velocidade/FPS,angulo);
					if (local.x+deslocamento.x > MAP_X_SIZE/2) {
						local.x = MAP_X_SIZE/2;
						velocidade = 0;
					} else if (local.x+deslocamento.x < -MAP_X_SIZE/2) {
						local.x = -MAP_X_SIZE/2;
						velocidade = 0;
					} else
						local.x = local.x+deslocamento.x;
						
					if (local.y+deslocamento.y > MAP_Y_SIZE/2) {
						local.y = MAP_Y_SIZE/2;
						velocidade = 0;
					} else if (local.y+deslocamento.y < -MAP_Y_SIZE/2) {
						local.y = -MAP_Y_SIZE/2;
						velocidade = 0;
					} else
						local.y = local.y+deslocamento.y;
					
					//atualiza a velocidade do tanque
					if (velocidade > 0)
						velocidade = MAX(velocidade-DESACELERACAO/FPS, 0);
					else
						velocidade = MIN(velocidade+DESACELERACAO/FPS, 0);
					
					//nao exige explicações
					SDL_RenderPresent(ren);
				}
			}
		} else if (escolha == MENU_TUTORIAL) {
			printf("Tutorial iniciado!");
		} else if (escolha == MENU_CREDITS) {
			printf("Bruno, Eraldo e Marcos");
		} else if (escolha == MENU_SAIR) {
			apprunning = 0;
		}
	}
	
	//finaliza a aplicação
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
