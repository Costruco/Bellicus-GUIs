#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "save.h"

void salvar(SaveData *data) {
    FILE *f = fopen("save.dat", "wb");
    if (!f) {
        perror("Erro ao salvar");
        return;
    }

    fwrite(data, sizeof(SaveData), 1, f);
    fclose(f);
}

int carregar(SaveData *data) {
    FILE *f = fopen("save.dat", "rb");
    if (!f) {
        return 0;  
    }

    fread(data, sizeof(SaveData), 1, f);
    fclose(f);
    return 1;
}

