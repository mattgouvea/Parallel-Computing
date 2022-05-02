#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "functions.h"
#include "timer.h"

double quad_adaptativa (struct parametro p) {

    double m, m1, m2; //pontos médios
    double fm, fm1, fm2; //valor de f nos pontos médios
    double area, area1, area2; //areas dos retângulos
    double erro;

    //calula os pontos médios
    m = (p.a+p.b)/2;
    m1 = (p.a+m)/2;
    m2 = (m+p.b)/2;

    //calcula o valor da função nos pontos médios
    fm = p.f(m);
    fm1 = p.f(m1);
    fm2 = p.f(m2);

    //calcula as areas dos retangulos
    area = (p.b-p.a)*fm;
    area1 = (m-p.a)*fm1;
    area2 = (p.b-m)*fm2;

    erro = fabs(area - (area1+area2)); //verifica o erro
    
    if (erro <= p.erro_max) {
        
        return area1+area2;
    }
    else {
        struct parametro p1 = {p.f, p.a, m, p.erro_max};
        struct parametro p2 = {p.f, m, p.b, p.erro_max};
        return quad_adaptativa(p1) + quad_adaptativa(p2);
    }
}

int main (int argc, char *argv[]) {
    double i;
    double ini, fim;
    struct parametro parametros;

    if(argc < 5) {
        printf("Use: <número da função> <limite a esquerda> <limite da direita> <erro máximo>\n");
        exit(1);
    }
    GET_TIME(ini);

    parametros = atribui_parametros(atoi(argv[1]), atof(argv[2]), atof(argv[3]), atof(argv[4]));
    i = quad_adaptativa(parametros);

    GET_TIME(fim);
    printf("I = %lf\n", i );
    printf("Tempo  = %lf\n",fim-ini);
    return 0;
}
