#ifndef QUADTREE_H
#define QUADTREE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define CAPACIDADE_QT 4
#define TAM_MAX_ARRAY 1024

//Axis Aligned Bounding Box (Caixa retangular que envolve a partícula)
typedef struct AABB{
    SDL_FPoint* centro;
    float meia_dimensao;
} AABB;

typedef struct QuadTree{
    AABB* atual;
    SDL_FPoint** pontos;

    //regioes do retangulo
    struct QuadTree* Noroeste;
    struct QuadTree* Nordeste;
    struct QuadTree* Sudoeste;
    struct QuadTree* Sudeste;

} QuadTree;

SDL_FPoint* novo_ponto(float x, float y);
SDL_FPoint* origem();
void printar_ponto(SDL_FPoint *point);
AABB* nova_AABB(SDL_FPoint* centro, float meia_dimensao);
bool ponto_em_AABB(AABB* atual, SDL_FPoint* ponto);
bool AABB_intersecao_AABB(AABB* atual, AABB* outro);
void printar_AABB(AABB* atual);
QuadTree* nova_QT(AABB* atual);
size_t qtd_pontos_QT(SDL_FPoint *pontos[]);
QuadTree* subdividir_QT(QuadTree* raiz);
bool inserir_QT(QuadTree* raiz, SDL_FPoint* ponto);
SDL_FPoint** query_range_QT(QuadTree* raiz, AABB* range);
void liberar_QT(QuadTree* qt);


#endif