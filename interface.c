#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <stdio.h>
#include <string.h>

#include "interface.h"

void infoBox(SDL_Renderer * ren, double x, double y, double w, double h) {
	rectangleRGBA(ren,x,y,x+w+1,y+h+1,255,255,255,255);
	boxRGBA(ren,x+1,y+1,x+w-1,y+h-1,0,0,0,255);
}

void infoLabel(SDL_Renderer * ren, double x, double y, char string[], double data) {
	char newString[strlen(string)+1];
	sprintf(newString,string,data);
	stringRGBA(ren,x+3,y+3,newString,255,255,255,255);
}

