#ifndef stack
#define stack

/* header com a implementação das estruturas de dados.
   uma struct que armazana os parâmetros usados na chamada
   da função recursiva que calcula a quadratura, e a estrutura
   e os métodos necessários para a implementação dos mecanismos
   de pilha usando array e usando lista encadeada  */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define TAM 1000000 //tamanho da pilha baseada em array

struct node *top = NULL; //ponteiro para o topo da pilha baseada em lista encadeada
int top_arr=-1; //variável que indica o topo da pilha baseada em array


//struct com os parâmetros da função recursiva quad_adaptativa
struct parametro
{
    double (*f) (double);
    double a;
    double b;
    double erro_max;
};
struct parametro pilha_arr[TAM]; //vetor da pilha com array

//struct que representa um nó na pilha com lista encadeada
struct node
{
    struct parametro p;
    struct node *next;
};

/* ===== implementação da pilha com lista encadeada ===== */

//método que adiciona um elemento na pilha.
void push(struct parametro p) {
    struct node *temp = (struct node*)malloc(sizeof(struct parametro));
    temp->p = p;
    temp->next = top;
    top = temp;
}

//método que remove o elemento no topo da pilha e retorna esse elemento
struct parametro pop() {
    struct node *temp;
    if (top == NULL) {printf("Pilha está vazia\n"); exit(-1);}
    temp = top;
    top = top->next;
    struct parametro p = temp->p;
    free(temp);
    return p;
}

/* ===== implementação da pilha usando array ===== */

//verifica se a pilha está vazia
int esta_vazia() {
    return top_arr == -1;
}

//verifica se a pilha está cheia
int esta_cheia() {
    return top_arr == TAM-1;
}

//insere um elemento na pilha
void push_arr(struct parametro p) {
    if (esta_cheia()) {
        printf("Pilha está cheia\n"); exit(1);
    }
    top_arr++;
    pilha_arr[top_arr] = p;
}

//retira um elemento da pilha e o retorna
struct parametro pop_arr() {
    struct parametro temp;
    if (esta_vazia()) {
        printf("Pilha está vazia\n"); exit(1); 
    }
    temp = pilha_arr[top_arr];
    top_arr--;
    return temp;
}

#endif