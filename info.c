#include "info.h"
	
int stringFormatSize(char * string) {
	int i = 0, n = 0, lendoFormato = 0, total = 0;
	while (string[i] != '\0') {
		switch (lendoFormato) {
			case 0:
				if (string[i] == '%') {
					lendoFormato = 1;
					n--;
				}	
				break;
			case 1:
				if (string[i] >= '0' && string[i] <= '9') {
					total *= 10;
					total += string[i]-'0';
					n--;
				} else {
					n += total-1;
					total = 0;
					lendoFormato = 2;
				}
				break;
			case 2:
				if (string[i] == ' ')
					lendoFormato = 0;
				else if (string[i] == '%')
					lendoFormato = 1;
				else
					n--;
				break;
		}
		i++;
		n++;
	}
	return n;
}

void dataBox(SDL_Renderer * ren, int x, int y, int max_nLetras, int nLinhas) {
	boxRGBA(ren,x,y,x+max_nLetras*8+5,y+nLinhas*11-1+3,0,0,0,255);
	rectangleRGBA(ren,x,y,x+max_nLetras*8+6,y+nLinhas*11+3,255,255,255,255);
}

void doubleDataLabel(SDL_Renderer * ren, int x, int y, int nPalavras, char * strings[], double data[], SDL_Color cores[]) {
    int max_nLetras = 0, nLinhas = nPalavras;
    for (int i = 0; i < nPalavras; i++) {
        size_t len = strlen(strings[i]);
        int new_max = stringFormatSize(strings[i]);
        if (new_max > max_nLetras) max_nLetras = new_max;
        if (len > 0 && strings[i][len-1] == '-')
            nLinhas--;
    }

    if (nLinhas < 1) nLinhas = 1;
    dataBox(ren, x, y, max_nLetras, nLinhas);

    for (int i = 0, wc = 0; wc < nPalavras; wc++) {
        size_t slen = strlen(strings[wc]);
        if (slen == 0) {
            i++;
            continue;
        }

        int concat = 0;
        if (strings[wc][slen - 1] == '-') concat = 1;

        char newString[256];
        int written = snprintf(newString, sizeof(newString), strings[wc], data[wc]);

        if (concat && written > 0) {
            size_t nslen = strlen(newString);
            if (nslen > 0) newString[nslen - 1] = ' ';
        }

        if (cores != NULL)
            stringRGBA(ren, x+3, y+3 + i*11, newString, cores[wc].r, cores[wc].g, cores[wc].b, cores[wc].a);
        else
            stringRGBA(ren, x+3, y+3 + i*11, newString, 255,255,255,255);

        i -= concat; 
        i++;
    }
}

void stringDataLabel(SDL_Renderer * ren, int x, int y, int nLinhas, char * string, char * data, SDL_Color * cor) {
    char newString[256];
    snprintf(newString, sizeof(newString), string, data);
    if (cor != NULL)
        stringRGBA(ren, x+3, y+3+nLinhas*11, newString, cor->r, cor->g, cor->b, cor->a);
    else
        stringRGBA(ren, x+3, y+3+nLinhas*11, newString, 255,255,255,255);
}

