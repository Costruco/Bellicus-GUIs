#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include "quadtree.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 400

void desenhar_ponto(SDL_Renderer* renderer, SDL_FPoint* ponto) {
    SDL_RenderDrawPointF(renderer, ponto->x, ponto->y);
}

void desenhar_AABB(SDL_Renderer* renderer, AABB* aabb) {
    SDL_FRect r;
    r.x = aabb->centro->x - aabb->meia_dimensao;
    r.y = aabb->centro->y - aabb->meia_dimensao;
    r.w = aabb->meia_dimensao * 2;
    r.h = aabb->meia_dimensao * 2;
    SDL_RenderDrawRectF(renderer, &r);
}

void desenhar_QT(SDL_Renderer* renderer, QuadTree* qt) {
    if (!qt) return;

    desenhar_AABB(renderer, qt->atual);

    for (size_t i = 0; i < CAPACIDADE_QT; i++) {
        if (qt->pontos[i]) {
            desenhar_ponto(renderer, qt->pontos[i]);
        }
    }

    desenhar_QT(renderer, qt->Noroeste);
    desenhar_QT(renderer, qt->Nordeste);
    desenhar_QT(renderer, qt->Sudoeste);
    desenhar_QT(renderer, qt->Sudeste);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("QuadTree Visual",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_FPoint* centro = novo_ponto(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
    float tamanho_inicial = SCREEN_WIDTH / 2.0f;
    AABB* area_inicial = nova_AABB(centro, tamanho_inicial);
    QuadTree* qt = nova_QT(area_inicial);

    SDL_Event evento;

    while (!SDL_QuitRequested()) {
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_MOUSEBUTTONDOWN) {
                float x = evento.button.x;
                float y = evento.button.y;
                SDL_FPoint* ponto = novo_ponto(x, y);
                inserir_QT(qt, ponto);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        desenhar_QT(renderer, qt);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // quase 60 FPS
    }

    liberar_QT(qt);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
