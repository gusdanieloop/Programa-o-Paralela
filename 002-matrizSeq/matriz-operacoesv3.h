#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include "matrizv3.h"

#ifndef SOME_HEADER_FILE_H
#define SOME_HEADER_FILE_H
typedef struct {
  int lin_inicio;
  int lin_fim;
  int col_inicio;
  int col_fim;
} bloco_t;

typedef struct {
  int** matriz;
  int lin;
  int col;
} mymatriz;

typedef struct {
  mymatriz* matriz;
  bloco_t* bloco;
} matriz_bloco_t;

#endif

typedef struct {
  int tid; //thread id
  int ntasks; //nro de tarefas
  int lfim; //linha final
  int lini; //linha inicial
  mymatriz* mat_a;
  mymatriz* mat_b;
  mymatriz* mat_cPar;
} param_t;

typedef struct {
  int tid; //thread id
  int ntasks; //nro de tarefas
  int blini;
  int blfim;
  pthread_mutex_t* lock;
  mymatriz* resultado;
  matriz_bloco_t** mat_a;
  matriz_bloco_t** mat_b;
  matriz_bloco_t** mat_cPar;
} param_bloco_t;


mymatriz* msomar (mymatriz* mat_a, mymatriz* mat_b, int tipo);
mymatriz* mmultiplicar (mymatriz* mat_a, mymatriz* mat_b, int tipo);
void* multiplicarth (void* arg);
void* multiplicarthbloco(void* arg);

int mmsubmatriz (matriz_bloco_t* mat_suba, matriz_bloco_t* mat_subb, matriz_bloco_t* mat_subc);
matriz_bloco_t** csubmatrizv2(int mat_lin, int mat_col, int divisor);
matriz_bloco_t** particionar_matriz(int** matriz, int mat_lin, int mat_col, int orientacao, int divisor);
void print_submatriz(matriz_bloco_t* mat);
