#ifndef INTERFACE_H
#define INTERFACE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <stdio.h>

int stringFormatSize(char * string);
void dataBox(SDL_Renderer * ren, int x, int y, int max_nLetras, int nLinhas);
void doubleDataLabel(SDL_Renderer * ren, int x, int y, int nLinhas, int nPalavras, char * strings[], double data[], SDL_Color cores[]);
void stringDataLabel(SDL_Renderer * ren, int x, int y, int nLinhas, char * string, char * data, SDL_Color * cor);

#endif
