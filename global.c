#include "global.h"

//velocidades
SDL_FPoint velocidades[] = {{-65,0},
 						    {0,0},
							{0,65},
							{65,135},
							{135,225},
							{225,487}};
						
//caixas de colis�o
SDL_FPoint vertices_tanque[] = {{-90,-50},{+90,-50},{+90,+50},{-90,+50}},
		   vertices_soldado[] = {{-16,-16},{+16,-16},{+16,+16},{-16,+16}};
		   
poligono hitbox_tanque = {4,
						  {0,0},
						  vertices_tanque},
	     hitbox_soldado = {4,
	                       {0,0},
						   vertices_soldado};
						   
//o FPS � GLOBAL!										  
int FPS = 120, TPF = 8,
	WIDTH,HEIGHT,
	MWIDTH,MHEIGHT;

//Tela de morte
EstadoDoJogo estado_jogo = JOGO_VIVO;
float fade_morte = 0.0f;
float zoom_morte = 0.3f;
int flash_alpha = 255;
Uint32 tempo_morte = 0;
