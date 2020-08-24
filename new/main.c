#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <omp.h>

#include "matriz.h"
#include "matriz_op.h"

int main(int argc, char *argv[]) {
    FILE* f;
    struct timespec start, end;
    int n = 1000, m = 1000;
    int qnt = 2;
    int n_threads = 8;
    init_random();

    if(n_threads > qnt){
        n_threads = qnt;
    }
    
    pthread_t* threads = (pthread_t*) malloc(n_threads * sizeof(pthread_t));
    
    matrix* m_a = m_alloc(n, m);
    m_reset(m_a, -1);
    f = fopen("A.map","w");
    m_file(m_a, f);
    fclose(f);
    matrix** b_a = divide(m_a, 1, qnt);
    //printf("MATRIZ A\n");
    //m_print(m_a);
    matrix* m_b = m_alloc(m, n);
    m_reset(m_b, -1);
    f = fopen("B.map","w");
    m_file(m_b, f);
    fclose(f);
    //printf("MATRIZ B\n");
    //m_print(m_b);
    matrix** b_b = divide(m_b, 0, qnt);
    matrix* m_c;
     
    printf("soma seq\n");
    //start_soma = clock();
    //end_soma = clock();
    //time = ((double) (end_soma - start_soma)) / CLOCKS_PER_SEC;
    //printf("levou %f\n", time);
    //m_print(m_c);
    clock_gettime(CLOCK_REALTIME, &start); 
    m_c = m_plus(m_a, m_a);
    //end_bloco = clock();
    //time = ((double) (end_bloco - start_bloco)) / CLOCKS_PER_SEC;
    clock_gettime(CLOCK_REALTIME, &end); 
    long seconds = end.tv_sec - start.tv_sec; 
    long ns = end.tv_nsec - start.tv_nsec;
    if (start.tv_nsec > end.tv_nsec) { // clock underflow 
	    --seconds; 
	    ns += 1000000000; 
    }  
    printf("levou %f\n", (double)seconds + (double)ns/(double)1000000000);

	f = fopen("soma_seq.result","w");
    m_file(m_c, f);
    fclose(f);
    m_free(m_c);
    
    printf("\nmult seq\n");
    //start_seq = clock();
    //end_seq = clock();
    //time = ((double) (end_seq - start_seq)) / CLOCKS_PER_SEC;
    //printf("levou %f\n", time);
    //m_print(m_c);
    clock_gettime(CLOCK_REALTIME, &start); 
    m_c = m_mult(m_a, m_b);
    //end_bloco = clock();
    //time = ((double) (end_bloco - start_bloco)) / CLOCKS_PER_SEC;
    clock_gettime(CLOCK_REALTIME, &end); 
    seconds = end.tv_sec - start.tv_sec; 
    ns = end.tv_nsec - start.tv_nsec;
    if (start.tv_nsec > end.tv_nsec) { // clock underflow 
	    --seconds; 
	    ns += 1000000000; 
    }  
    printf("levou %f\n", (double)seconds + (double)ns/(double)1000000000);
    f = fopen("mult_seq.result","w");
    m_file(m_c, f);
    fclose(f);
    m_free(m_c);
    
    printf("\nmult bloco\n");
    //start_bloco = clock();
    clock_gettime(CLOCK_REALTIME, &start); 
    m_c = b_mult(b_a, b_b, qnt);
    //end_bloco = clock();
    //time = ((double) (end_bloco - start_bloco)) / CLOCKS_PER_SEC;
    clock_gettime(CLOCK_REALTIME, &end); 
    seconds = end.tv_sec - start.tv_sec; 
    ns = end.tv_nsec - start.tv_nsec;
    if (start.tv_nsec > end.tv_nsec) { // clock underflow 
	    --seconds; 
	    ns += 1000000000; 
    }  
    printf("levou %f\n", (double)seconds + (double)ns/(double)1000000000);
    //printf("levou %f\n", time);
    //m_print(m_c);
    f = fopen("mult_bloco.result","w");
    m_file(m_c, f);
    fclose(f);
    m_free(m_c);
    for(int i = 0; i < qnt; ++i) {
        m_free(b_a[i]);
        m_free(b_b[i]);
    }

    printf("\nmult thread\n");
    th_args** args = t_alloc(m_a, m_b, qnt, n_threads);
    
    matrix** t_plus = (matrix**) malloc(n_threads * sizeof(matrix*));
    for(int i = 0; i < n_threads; ++i) {
        t_plus[i] = (matrix*) malloc(sizeof(matrix));
    }
    
    clock_gettime(CLOCK_REALTIME, &start); 
    //start_thread = clock();
    for(int i = 0; i < n_threads; ++i){
        pthread_create(&threads[i], NULL, th_mult, (void*)args[i]);
    }
    for (int i = 0; i < n_threads; i++)
    {
        pthread_join(threads[i], NULL);
        t_plus[i] = args[i]->b_c;
    }
    m_c = b_plus(t_plus, n_threads);
    clock_gettime(CLOCK_REALTIME, &end); 
    seconds = end.tv_sec - start.tv_sec; 
    ns = end.tv_nsec - start.tv_nsec;
    if (start.tv_nsec > end.tv_nsec) { // clock underflow 
	    --seconds; 
	    ns += 1000000000; 
    }  
    printf("levou %f\n", (double)seconds + (double)ns/(double)1000000000);
    f = fopen("mult_thread.result","w");
    m_file(m_c, f);
    fclose(f);
    m_free(m_c);
    for(int i = 0; i < n_threads; ++i){
        m_free(t_plus[i]);
        for(int j = 0; j < args[i]->qnt; ++j){
            m_free(args[i]->b_a[j]);
            m_free(args[i]->b_b[j]);
        }
    }


    printf("\nmult omp\n");
    args = t_alloc(m_a, m_b, qnt, n_threads);

    t_plus = (matrix**) malloc(n_threads * sizeof(matrix*));
    for(int i = 0; i < n_threads; ++i) {
        t_plus[i] = (matrix*) malloc(sizeof(matrix));
    }
    
    clock_gettime(CLOCK_REALTIME, &start);
    int i; 
    #pragma omp parallel for private(i)  
    for(i = 0; i < n_threads; ++i){
        th_mult((void*)args[i]);
        t_plus[i] = args[i]->b_c;
    }
    m_c = b_plus(t_plus, n_threads);
    clock_gettime(CLOCK_REALTIME, &end); 
    seconds = end.tv_sec - start.tv_sec; 
    ns = end.tv_nsec - start.tv_nsec;
    if (start.tv_nsec > end.tv_nsec) { // clock underflow 
	    --seconds; 
	    ns += 1000000000; 
    }  
    printf("levou %f\n", (double)seconds + (double)ns/(double)1000000000);
    f = fopen("mult_omp.result","w");
    m_file(m_c, f);
    fclose(f);
    m_free(m_c);
    m_free(m_a);
    m_free(m_b);
    
    
    //end_thread = clock();
    //time = ((double) (end_thread - start_thread)) / CLOCKS_PER_SEC;
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