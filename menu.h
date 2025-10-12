#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>

typedef enum {
    MENU_PLAY,
    MENU_TUTORIAL,
    MENU_CREDITS,
    MENU_SAIR,
    MENU_TOTAL
} MenuOption;

MenuOption menu_loop(SDL_Renderer* renderer, SDL_Window* window, int width, int height);

#endif
