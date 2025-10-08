#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <stdio.h>
#include <string.h>

#include "interface.h"

void infoBox(SDL_Renderer * ren, int x, int y, int max_nLetras, int nLinhas) {
	boxRGBA(ren,x,y,x+max_nLetras*8+5,y+nLinhas*11-1+3,0,0,0,255);
	rectangleRGBA(ren,x,y,x+max_nLetras*8+6,y+nLinhas*11+3,255,255,255,255);
}

void infoLabel(SDL_Renderer * ren, int x, int y, int max_nLetras, int nLinhas, int nPalavras, char * strings[], double data[], SDL_Color cores[]) {
	infoBox(ren,x,y,max_nLetras,nLinhas);
	for (int i = 0, wc = 0; wc < nPalavras; i++, wc++) {
		int concat = 0;
		if (strings[wc][strlen(strings[wc])-1] == '-') {
			concat = 1;
		}
		char newString[strlen(strings[wc])+1];
		sprintf(newString,strings[wc],data[wc]);
		if (concat)
			newString[strlen(newString)-1] = ' ';
		if (cores != NULL)
			stringRGBA(ren,x+3,y+3+i*11,newString,cores[wc].r,cores[wc].g,cores[wc].b,cores[wc].a);
		else
			stringRGBA(ren,x+3,y+3+i*11,newString,255,255,255,255);
		i -= concat;
	}
}

