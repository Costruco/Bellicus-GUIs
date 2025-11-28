#ifndef LOGICA_H
#define LOGICA_H

#define MAX(a,b) (a>b)?a:b
#define MIN(a,b) (a<b)?a:b

#include "aritmetica.h"

int numeroDentroIntervalo(double n, double a, double b);
int anguloDentroIntervalo(double n, double a, double b);
int aproxIgual(double n, double np);

#endif
