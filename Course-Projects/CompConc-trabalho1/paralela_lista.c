#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#include "estruturas.h"
#include "functions.h"
#include "timer.h"

double quad_adaptativa(struct parametro p);

double inicio, fim;
int calculando=0; //variável que indica quantas threads estão ainda executando
int nthreads, func; //número de threads e váriavel que recebe o número da função que se deseja avaliar a área, respectivamente
pthread_mutex_t mutex;
pthread_cond_t thread_executando; //variável de sincronização condicional, que representa a condição de que existe
                                  //thread ainda executando

struct parametro pega_proximo_elemento(double *ret, double soma_local) {
    while(top == NULL && calculando) { //se a pilha tiver vazia e tiver alguém calculando, espera o calculo terminar
        pthread_cond_wait(&thread_executando, &mutex); //espera na condição de que existe thread executando
        if(top == NULL && !calculando) { //se a pilha tiver vazia e não tiver ninguém calculando, retorna
            pthread_mutex_unlock(&mutex);
            *ret = soma_local;
            pthread_exit((void*) ret);
        }
    }
    calculando++; //sinalizando que tem alguém calculando
    struct parametro p = pop();
    return p;
}

//função chamada pelas threads
void *calcula_thread(void *args) {
    double *ret; //variável de retorno em pthread_exit
    ret = malloc(sizeof(double)); if (ret == NULL) exit(-1);  
    struct parametro p; //struct com os argumentos necessários pra chamar a função
    double soma_local = 0;
    pthread_mutex_lock(&mutex);  
    while(top != NULL || calculando) { //enquanto a pilha tiver algum elemento ou alguma thread ainda tiver calculando
        
        p = pega_proximo_elemento(ret, soma_local); //pega o próximo elemento na pilha ou retorna a soma local
        pthread_mutex_unlock(&mutex);
        
        soma_local += quad_adaptativa(p); //chama a função recursiva
        pthread_mutex_lock(&mutex);
        calculando--; //sinaliza que terminou de calcular
        pthread_cond_broadcast(&thread_executando); //acorda as outras threads
    }
    pthread_mutex_unlock(&mutex);

    *ret = soma_local;
    pthread_exit((void *) ret);
}

double quad_adaptativa (struct parametro p) {

    double m, m1, m2; //pontos médios
    double fm, fm1, fm2; //valor da função nos pontos médios
    double area, area1, area2; //areas dos retângulos
    double erro;

    //calula os pontos médios
    m = (p.a+p.b)/2;
    m1 = (p.a+m)/2;
    m2 = (m+p.b)/2;

    //calcula o valor das funções nos pontos médios
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
        pthread_mutex_lock(&mutex);
        push(p1); //põe um retângulo na pilha
        pthread_cond_signal(&thread_executando); //sinaliza alguma thread que esteja esperando que tem um novo elemento
        pthread_mutex_unlock(&mutex);
        return quad_adaptativa(p2); //calcula o outro retângulo
    }
}

int main (int argc, char *argv[]) {
    pthread_t tid_sistema[nthreads];
    int *tid; //guarda a id da thread
    int i;
    double area_total=0; //resultado da integração
    double *retorno; //pra pegar o retorno das threads
    retorno = malloc(sizeof(double));
    struct parametro p_inicial; //variável com os argumentos dados pelo usuário

    if(argc < 6) {
        printf("Use: <número da função> <limite a esquerda> <limite da direita> <erro máximo> <número de threads>\n");
        exit(1);
    }

    nthreads = atoi(argv[5]);

    //atribui os argumentos a estrutura usada para acomodar as entradas do usuário
    p_inicial = atribui_parametros(atoi(argv[1]), atof(argv[2]), atof(argv[3]), atof(argv[4]));
    push(p_inicial); //põe o primeiro elemento na pilha

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&thread_executando, NULL);
    
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
        area_total += *(double *)retorno; //soma as áreas cálculadas pelas threads
        free(retorno);
    }
    GET_TIME(fim);
    printf("Resultado = %lf\n", area_total);
    printf("Tempo total = %lf\n", fim - inicio);
    return 0;
}