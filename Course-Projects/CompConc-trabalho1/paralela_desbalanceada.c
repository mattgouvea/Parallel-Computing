#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#include "estruturas.h"
#include "functions.h"
#include "timer.h"

double quad_adaptativa(struct parametro p);

double inicio, fim; //variáveis para a contagem do tempo de execução
int nthreads, func; //número de threads e o número que representa a função que se deseja calcular a área, repectivamente
double a, b, erro; //limite a esquerda, limite a direita e erro tolorável, respectivamente
struct parametro *vet; //vetor para guardar os parâmetros passados para cada thread

//função chamada pelas threads
void *calcula_thread(void *args) {
    int id = *(int *) args; //identificação da thread
    double *ret; //variável de retorno em pthread_exit
    ret = malloc(sizeof(double)); if (ret == NULL) exit(-1);  

    *ret = quad_adaptativa(vet[id]); //calcula o intervalo fornecido, como cada thread tem seu índice
                                     //não há necessidade de mutex

    pthread_exit((void *) ret);
}

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
    
    int *tid;
    int i;
    double area_total=0, b_aux, a_aux; //resultado da integração
    double *retorno; //pra pegar o retorno das threads
    retorno = malloc(sizeof(double)); if(retorno == NULL) {printf("Erro no malloc\n"); exit(1);}
    struct parametro p_inicial;

    if(argc < 6) {
        printf("Use: <número da função> <limite a esquerda> <limite da direita> <erro máximo> <número de threads>\n");
        exit(1);
    }
    
    a = atof(argv[2]); a_aux = a;
    b = atof(argv[3]);
    erro = atof(argv[4]);
    nthreads = atoi(argv[5]);

    pthread_t tid_sistema[nthreads];

    //atribui os argumentos ao vetor que guarda os parâmetros que serão passados para as threads
    vet = (struct parametro *) malloc(sizeof(struct parametro)*nthreads);
    if (vet == NULL) {printf("Falha no malloc de vet\n"); exit(1);}

    for(i=0; i<nthreads; i++) {
        b_aux = a_aux+((b-a)/nthreads);
        //cada índice contém o intervalo que será passado para sua respectiva thread
        vet[i] = atribui_parametros(atoi(argv[1]), a_aux, b_aux, erro);
        a_aux = b_aux;
    }

    GET_TIME(inicio);
    for(i=0; i<nthreads; i++){
        tid = malloc(sizeof(int));
        *tid = i;
        if (pthread_create(&tid_sistema[i], NULL, calcula_thread, (void *) tid)) {
            printf("Erro na criação da thread.\n"); exit(-1);
        }
    }

    for (i=0; i<nthreads; i++) {
        if (pthread_join(tid_sistema[i], (void *) &retorno)) {
            printf("Erro na pthread_join.\n"); exit(-1);
        }
        area_total += *(double *)retorno;
        free(retorno);
    }
    GET_TIME(fim);
    printf("Resultado = %lf\n", area_total);
    printf("Tempo total = %lf\n", fim - inicio);
    return 0;
}