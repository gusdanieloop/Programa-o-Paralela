#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "matriz.h"
#include "matriz_op.h"

int main(int argc, char *argv[]) {
    clock_t start_soma, end_soma, start_seq, end_seq, start_bloco, end_bloco, start_thread, end_thread;
    double time;
    int n = 30, m = 30;
    int qnt = 100;
    int n_threads = 4;
    init_random();

    if(n_threads > qnt){
        n_threads = qnt;
    }
    
    pthread_t* threads = (pthread_t*) malloc(n_threads * sizeof(pthread_t));
    
    matrix* m_a = m_alloc(1000, 1000);
    m_reset(m_a, 1);
    matrix** b_a = divide(m_a, 1, qnt);
    //printf("MATRIZ A\n");
    //m_print(m_a);
    matrix* m_b = m_alloc(1000, 1000);
    m_reset(m_b, 1);
    //printf("MATRIZ B\n");
    //m_print(m_b);
    matrix** b_b = divide(m_b, 0, qnt);
    matrix* m_c;
    /*
    printf("soma seq\n");
    start_soma = clock();
    m_c = m_plus(m_a, m_a);
    end_soma = clock();
    time = ((double) (end_soma - start_soma)) / CLOCKS_PER_SEC;
    printf("levou %f\n", time);
    //m_print(m_c);

    printf("\nmult seq\n");
    start_seq = clock();
    m_c = m_mult(m_a, m_b);
    end_seq = clock();
    time = ((double) (end_seq - start_seq)) / CLOCKS_PER_SEC;
    printf("levou %f\n", time);
    //m_print(m_c);

    printf("\nmult bloco\n");
    start_bloco = clock();
    m_c = b_mult(b_a, b_b, qnt);
    end_bloco = clock();
    time = ((double) (end_bloco - start_bloco)) / CLOCKS_PER_SEC;
    printf("levou %f\n", time);
    //m_print(m_c);
    
    */
    printf("\nmult thread\n");
    th_args** args = t_alloc(m_a, m_b, qnt, n_threads);
    
    matrix** t_plus = (matrix**) malloc(n_threads * sizeof(matrix*));
    for(int i = 0; i < n_threads; ++i) {
        t_plus[i] = (matrix*) malloc(sizeof(matrix));
    }
    
    start_thread = clock();
    for(int i = 0; i < n_threads; ++i){
        pthread_create(&threads[i], NULL, th_mult, (void*)args[i]);
    }
    for (int i = 0; i < n_threads; i++)
    {
        pthread_join(threads[i], NULL);
        t_plus[i] = args[i]->b_c;
    }
    m_c = b_plus(t_plus, n_threads);
    end_thread = clock();
    time = ((double) (end_thread - start_thread)) / CLOCKS_PER_SEC;
    printf("levou %f\n", time);
    //m_print(m_c);
    
    //TESTE DIFERENÇA VELOCIDADE MATRIZ DUAS DIMENSÕES E UMA DIMENSÃO
    /*int mat1[n][n];
    int mat2[n*n];
    for(int i = 0; i < n; ++i)
        for(int j = 0; j < n; ++j){
            mat1[i][j] = i * j;
            mat2[j + (n * i)] = i * j;
        }*/
    /*start = clock();
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            printf("%d ", mat1[i][j]);
        }
        printf("\n");
    }
    end = clock();
    time = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("levou %f\n", time);


    start = clock();
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            printf("%d ", mat2[j + (n * i)]);
        }
        printf("\n");
    }
    end = clock();
    time = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("levou %f\n", time);*/
    //UMA DIMENSÃO É DUAS VEZES MAIS RÁPIDO
    return 0;
}