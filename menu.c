#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#include "menu.h"

int AUX_WaitEventTimeoutCount(SDL_Event* evt, Uint32 * ms) {
	Uint32 antes = SDL_GetTicks();
	int isevt = SDL_WaitEventTimeout(evt, *ms);
	Uint32 deltaEspera = SDL_GetTicks()-antes, novaEspera = *ms-deltaEspera;
	if (novaEspera > *ms)
		*ms = 0;
	else
		*ms -= deltaEspera;
	return isevt;
}

MenuOption menu_loop(SDL_Renderer* renderer, SDL_Window* win) {
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
        setaRect.x = 880; 
        setaRect.y = 335 + selected * 90;
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
