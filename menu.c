#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#include "espera.h"
#include "menu.h"

MenuOption menu_loop(SDL_Renderer* renderer, SDL_Window* win, int width, int height) {
    SDL_Surface* tempSurface = IMG_Load("./img/bellicus_inicio.png");
    SDL_Texture* background = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    SDL_Surface* setaSurface = IMG_Load("./img/seta.png");
    SDL_Texture* setaTex = SDL_CreateTextureFromSurface(renderer, setaSurface);
    SDL_FreeSurface(setaSurface);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erro ao iniciar SDL_mixer: %s\n", Mix_GetError());
        return MENU_TOTAL; 
    }

    Mix_Music* music = Mix_LoadMUS("./music/hino_urss.mp3");
    if (!music) {
        printf("Erro ao carregar música: %s\n", Mix_GetError());
    } else {
        Mix_PlayMusic(music, -1); 
    }


    SDL_Event e;
    MenuOption selected = MENU_PLAY;
    Uint32 espera = 25;

    while (!SDL_QuitRequested()) {
        int isevt = AUX_WaitEventTimeoutCount(&e, &espera);
         if(isevt){
             if (e.type == SDL_KEYDOWN) {
                            switch (e.key.keysym.sym) {
                                case SDLK_UP:
                                    selected = (MenuOption)((selected - 1 + MENU_TOTAL) % MENU_TOTAL);
                                    break;
                                case SDLK_DOWN:
                                    selected = (MenuOption)((selected + 1) % MENU_TOTAL);
                                    break;
                                case SDLK_RETURN:
                                    SDL_DestroyTexture(background);
                                    SDL_DestroyTexture(setaTex);

                                    Mix_HaltMusic();
                                    Mix_FreeMusic(music);
                                    Mix_CloseAudio();

                                    return selected;
                            }
                        }
                }
        
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);

        SDL_Rect setaRect;
        setaRect.w = 40; 
        setaRect.h = 40; 
        setaRect.x = 1.32*width/2; 
        setaRect.y = (height/2.22) + selected * height/9;
        SDL_RenderCopy(renderer, setaTex, NULL, &setaRect);

        SDL_RenderPresent(renderer);
    
    }

    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);

    Mix_HaltMusic();
    Mix_FreeMusic(music);
    Mix_CloseAudio();


    return MENU_TOTAL;
}
