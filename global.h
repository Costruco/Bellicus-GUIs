#ifndef GLOBAL_H
#define GLOABL_H

#include <SDL2/SDL.h>

#include "estados.h"
#include "estruturas.h"

#define MWIDTH   WIDTH/2
#define MHEIGHT HEIGHT/2

extern SDL_FPoint velocidades[];


extern SDL_FPoint vertices_tanque[],
		   vertices_soldado[];
extern poligono hitbox_tanque,
		 hitbox_soldado;

		 
extern int FPS,TPF,
		   WIDTH,HEIGHT;


extern EstadoDoJogo estado_jogo;
extern float fade_morte;
extern float zoom_morte;
extern int flash_alpha;
extern Uint32 tempo_morte;
		 
#endif
