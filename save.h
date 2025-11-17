#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

typedef struct SaveData {
    double vida_atual;
    int qtd_tripulantes;
    char** funcoes_tripulantes;
    int fase_atual;
    int armas_desbloqueadas;
    //[...] mais tipos se for necessário
} SaveData;
