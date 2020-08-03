#ifndef MATRIZ_HPP
#define MATRIZ_HPP

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct{
    int *datarow;
    int *datacol;
    int row;
    int col;
} matrix;

typedef struct{
    matrix** b_a;
    matrix** b_b;
    matrix* b_c;
    int qnt;
} th_args;

void init_random();
matrix* m_alloc(int row, int col);
matrix** divide(matrix* m, int orientacao, int qnt);
th_args** t_alloc(matrix* a, matrix* b, int qnt, int n_threads);
void m_reset(matrix* m, int value);
void m_print(matrix* m);
void m_free(matrix* m);
void m_file(matrix* m, FILE* f);

#endif