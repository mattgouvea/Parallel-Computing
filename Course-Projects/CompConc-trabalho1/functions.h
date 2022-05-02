#ifndef functions
#define functions

#include <math.h>
#include <stdio.h>

#include "estruturas.h"

/* funções determidas para aplicar o método da integração */

double f1 (double x) {
    return x + 1;
}

double f2 (double x) {
    if(-1<x && x<1) {
        return sqrt(1-x*x);
    }
    else  {
        printf("x tem que estar entre -1 e 1\n"); return -1;
    }
}

double f3 (double x) {
    return sqrt(1 + pow(x, 4));
}

double f4 (double x) {
    return sin(x*x);
}

double f5 (double x) {
    return cos(exp(-x));
}

double f6 (double x) {
    return cos(exp(-x))*x;
}

double f7 (double x) {
    return cos(exp(-x)) * (0.005*(x*x*x)+1);
}

/* funções de uso auxiliar */
struct parametro atribui_parametros_auxiliar (double (*f) (double), double a, double b, double erro) {
    struct parametro p = {f, a, b, erro};
    return p;
}


struct parametro atribui_parametros(int num_func, double a, double b, double erro) {
    struct parametro p;

    switch (num_func)
    {
    case 1:
        p = atribui_parametros_auxiliar(f1, a, b, erro);
        break;
    
    case 2:
        p = atribui_parametros_auxiliar(f2, a, b, erro);
        break;

    case 3:
        p = atribui_parametros_auxiliar(f3, a, b, erro);
        break;

    case 4:
        p = atribui_parametros_auxiliar(f4, a, b, erro);
        break;

    case 5:
        p = atribui_parametros_auxiliar(f5, a, b, erro);
        break;

    case 6:
        p = atribui_parametros_auxiliar(f6, a, b, erro);
        break;

    case 7:
        p = atribui_parametros_auxiliar(f7, a, b, erro);
        break;
    
    default:
        break;
    }

    return p;
}

#endif