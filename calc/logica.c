#include "logica.h"

int numeroDentroIntervalo(double n, double a, double b) {
	if (a < b)
		return (n >= a && n <= b)?1:0;
	return (n >= b && n <= a)?1:0;
}

int anguloDentroIntervalo(double n, double a, double b) {
	if (b >= 360 && n <= 180) {
		return numeroDentroIntervalo(n,a-360,limitarDouble(b,360));
	}
	else if (a <= 0 && n >= 180) {
		return numeroDentroIntervalo(n-360,a,b);
	}
	return numeroDentroIntervalo(n,a,b);
}

int aproxIgual(double n, double np) {
	return numeroDentroIntervalo(n,np-0.1,np+0.1);
}	
