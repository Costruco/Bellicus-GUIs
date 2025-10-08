#ifndef INTERFACE_H
#define INTERFACE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <stdio.h>

void infoBox(SDL_Renderer * ren, int x, int y, int max_nLetras, int nLinhas);
void infoLabel(SDL_Renderer * ren, int x, int y, int max_nLetras, int nLinhas, int nPalavras, char * strings[], double data[], SDL_Color cores[]);

#endif
