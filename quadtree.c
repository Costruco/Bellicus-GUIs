#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "quadtree.h"


SDL_FPoint* novo_ponto(float x, float y){
    SDL_FPoint* ponto = (SDL_FPoint*)malloc(sizeof(SDL_FPoint));
    ponto->x = x;
    ponto->y = y;
    return ponto;
}

SDL_FPoint* origem(){
    SDL_FPoint* ponto = (SDL_FPoint*)malloc(sizeof(SDL_FPoint));
    ponto->x = 0;
    ponto->y = 0;
    return ponto;
}

void printar_ponto(SDL_FPoint *point) {
    printf("(%2.2f, %2.2f)\n", point->x, point->y);
}

AABB* nova_AABB(SDL_FPoint* centro, float meia_dimensao){
    AABB* atual = (AABB*)malloc(sizeof(AABB));
    atual->centro = centro;
    atual->meia_dimensao = meia_dimensao;
    return atual;
} 

//checa se o ponto esta no retangulo
bool ponto_em_AABB(AABB* atual, SDL_FPoint* ponto){
    if(ponto->x < atual->centro->x - atual->meia_dimensao || ponto->x > atual->centro->x + atual->meia_dimensao) return false;

    if(ponto->y < atual->centro->y - atual->meia_dimensao || ponto->y > atual->centro->y + atual->meia_dimensao) return false;

    return true;

}

//checa se ha intersecao entre os retangulos proprios das particulas
bool AABB_intersecao_AABB(AABB* atual, AABB* outro){
    if (atual->centro->x + atual->meia_dimensao < outro->centro->x - outro->meia_dimensao) return false;
    if (atual->centro->x - atual->meia_dimensao > outro->centro->x + outro->meia_dimensao) return false;
    if (atual->centro->y + atual->meia_dimensao < outro->centro->y - outro->meia_dimensao) return false;
    if (atual->centro->y - atual->meia_dimensao > outro->centro->y + outro->meia_dimensao) return false;
    return true;
}

void printar_AABB(AABB* atual) {
    printf("\n");
    printf("(%5.2f, %5.2f)-------------------------(%5.2f, %5.2f)\n", atual->centro->x - atual->meia_dimensao, atual->centro->y + atual->meia_dimensao, atual->centro->x + atual->meia_dimensao, atual->centro->y + atual->meia_dimensao);
    printf("|                                                   |\n");
    printf("|                                                   |\n");
    printf("|                                                   |\n");
    printf("|                                                   |\n");
    printf("|                                                   |\n");
    printf("|                                                   |\n");
    printf("|                                                   |\n");
    printf("|                                                   |\n");
    printf("(%5.2f, %5.2f)--------------------------(%5.2f, %5.2f)\n", atual->centro->x - atual->meia_dimensao, atual->centro->y - atual->meia_dimensao, atual->centro->x + atual->meia_dimensao, atual->centro->y - atual->meia_dimensao);
    printf("\n");
}

QuadTree* nova_QT(AABB* atual){
    QuadTree* qt = (QuadTree*)malloc(sizeof(QuadTree));
    qt->atual = atual;
    
    //inicializa as regioes
    qt->Nordeste = NULL;
    qt->Noroeste = NULL;
    qt->Sudoeste = NULL;
    qt->Sudeste = NULL;

    //inicializa os pontos
    qt->pontos = (SDL_FPoint**)malloc(sizeof(SDL_FPoint*)* CAPACIDADE_QT);

    for(size_t i = 0;i<CAPACIDADE_QT;i++){
        qt->pontos[i] = NULL;
    }

    return qt;
}

size_t qtd_pontos_QT(SDL_FPoint *pontos[]) {
    size_t i;
    for (i = 0; i < CAPACIDADE_QT; i++)
    {
        if (pontos[i] == NULL) {
            return i;
        }
    }

    return i;
}

QuadTree* subdividir_QT(QuadTree* raiz){
    float meia_dimensao = raiz->atual->meia_dimensao/2.0f;
    //Noroeste
    SDL_FPoint* centro_noroeste = novo_ponto(raiz->atual->centro->x - meia_dimensao, raiz->atual->centro->y - meia_dimensao);
    raiz->Noroeste = nova_QT(nova_AABB(centro_noroeste, meia_dimensao));

    //Nordeste
    SDL_FPoint* centro_nordeste = novo_ponto(raiz->atual->centro->x + meia_dimensao, raiz->atual->centro->y - meia_dimensao);
    raiz->Nordeste = nova_QT(nova_AABB(centro_nordeste, meia_dimensao));

    //Sudoeste
    SDL_FPoint* centro_sudoeste = novo_ponto(raiz->atual->centro->x - meia_dimensao, raiz->atual->centro->y + meia_dimensao);
    raiz->Sudoeste = nova_QT(nova_AABB(centro_sudoeste, meia_dimensao));

    //Sudeste
    SDL_FPoint* centro_sudeste = novo_ponto(raiz->atual->centro->x + meia_dimensao, raiz->atual->centro->y + meia_dimensao);
    raiz->Sudeste = nova_QT(nova_AABB(centro_sudeste, meia_dimensao));

    return raiz;
}

bool inserir_QT(QuadTree* raiz, SDL_FPoint* ponto){
    //se o ponto nao esta em AABB
    if(!ponto_em_AABB(raiz->atual, ponto)) return false;

    //se o ponto esta em AABB
    size_t qtd_pontos = qtd_pontos_QT(raiz->pontos);

    //se nao estourou a capacidade
    if(qtd_pontos < CAPACIDADE_QT && raiz->Noroeste == NULL){
        raiz->pontos[qtd_pontos] = ponto;
        return true;
    }
    //se estourou a capacidade, subdividir
    if(raiz->Noroeste == NULL){
        subdividir_QT(raiz);

        // Redistribui os pontos existentes do no pai para os filhos
        for (size_t i = 0; i < qtd_pontos; i++) {
            SDL_FPoint* p = raiz->pontos[i];

            if (inserir_QT(raiz->Noroeste, p)) continue;
            if (inserir_QT(raiz->Nordeste, p)) continue;
            if (inserir_QT(raiz->Sudoeste, p)) continue;
            if (inserir_QT(raiz->Sudeste, p)) continue;
        }

        // Limpa os pontos do no pai, estao nos filhos agora
        for (size_t i = 0; i < qtd_pontos; i++) {
            raiz->pontos[i] = NULL;
        }
    }


    if(inserir_QT(raiz->Noroeste, ponto)) return true;
    if(inserir_QT(raiz->Nordeste, ponto)) return true;
    if(inserir_QT(raiz->Sudoeste, ponto)) return true;
    if(inserir_QT(raiz->Sudeste, ponto)) return true;

    return false;
}

SDL_FPoint** query_range_QT(QuadTree* raiz, AABB* range){
    SDL_FPoint** result = (SDL_FPoint**)malloc(sizeof(SDL_FPoint*) * TAM_MAX_ARRAY);
    for(size_t i = 0;i< TAM_MAX_ARRAY;i++) result[i] = NULL;

    size_t index = 0;

    if(!AABB_intersecao_AABB(raiz->atual, range)) return result;

    size_t qtd_pontos = qtd_pontos_QT(raiz->pontos);
    for(size_t i = 0;i<qtd_pontos && index < TAM_MAX_ARRAY;i++){
        if(ponto_em_AABB(range,raiz->pontos[i])){
            result[index++] = raiz->pontos[i];
        }
    }

    if(raiz->Noroeste == NULL){
        return result;
    }

    size_t i;

    i = 0;
    SDL_FPoint** noroeste_r = query_range_QT(raiz->Noroeste, range);
    while(noroeste_r[i] != NULL && i< TAM_MAX_ARRAY && index < TAM_MAX_ARRAY){
        result[index++] = noroeste_r[i];
        i++;
    }
    free(noroeste_r);

    i = 0;
    SDL_FPoint** nordeste_r = query_range_QT(raiz->Nordeste, range);
    while(nordeste_r[i] != NULL && i< TAM_MAX_ARRAY && index < TAM_MAX_ARRAY){
        result[index++] = nordeste_r[i];
        i++;
    }
    free(nordeste_r);

    i = 0;
    SDL_FPoint** sudoeste_r = query_range_QT(raiz->Sudoeste, range);
    while(sudoeste_r[i] != NULL && i< TAM_MAX_ARRAY && index < TAM_MAX_ARRAY){
        result[index++] = sudoeste_r[i];
        i++;
    }
    free(sudoeste_r);

    i = 0;
    SDL_FPoint** sudeste_r = query_range_QT(raiz->Sudeste, range);
    while(sudeste_r[i] != NULL && i< TAM_MAX_ARRAY && index < TAM_MAX_ARRAY){
        result[index++] = sudeste_r[i];
        i++;
    }
    free(sudeste_r);

    return result;
}

void liberar_QT(QuadTree* qt) {
    if (qt == NULL) return;

    // Libera recursivamente os filhos primeiro
    liberar_QT(qt->Noroeste);
    liberar_QT(qt->Nordeste);
    liberar_QT(qt->Sudoeste);
    liberar_QT(qt->Sudeste);

    // Libera os pontos armazenados neste no
    if (qt->pontos != NULL) {
        /*Alocando os pontos dinamicamente (com clique do mouse), precisamos
        desalocar eles tambem */
        
        for (size_t i = 0; i < CAPACIDADE_QT; i++) {
            if (qt->pontos[i] != NULL) {
                free(qt->pontos[i]);
            }
        }
        
        free(qt->pontos);
    }

    if (qt->atual != NULL) {
        if (qt->atual->centro != NULL) {
            free(qt->atual->centro);
        }
        free(qt->atual);
    }

    free(qt);
}




