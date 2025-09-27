#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef enum {
    MENU_PLAY,
    MENU_TUTORIAL,
    MENU_CREDITS,
    MENU_TOTAL
} MenuOption;

int AUX_WaitEventTimeoutCount(SDL_Event* evt, Uint32 * ms);
MenuOption menu_loop(SDL_Renderer* renderer, SDL_Window* window);

#endif
