#ifndef INTERFACE_H
#define INTERFACE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <stdio.h>

void infoBox(SDL_Renderer * ren, double x, double y, double w, double h);
void infoLabel(SDL_Renderer * ren, double x, double y, char string[], double data);

#endif
