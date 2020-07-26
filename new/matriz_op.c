#include <stdio.h>
#include <stdlib.h>

#include "matriz_op.h"

matrix* m_plus(matrix* a, matrix* b){
    if(a->col != b->col || a->row != b->row){
        return NULL;
    }
    matrix* c = m_alloc(a->row, b->col);
    for(int i = 0; i < c->row * c->col; ++i){
        c->datarow[i] = a->datarow[i] + b->datarow[i];
        c->datacol[i] = a->datacol[i] + b->datacol[i];
    }
    return c;
}

matrix* b_plus(matrix** a, int qnt){
    int ref_col = a[0]->col, ref_row = a[0]->row;
    for(int i = 0; i < qnt; ++i)
        if(a[i]->col != ref_col || a[i]->row != ref_row){
            return NULL;
        }
    matrix* c = m_alloc(a[0]->row, a[0]->col);
    m_reset(c, 0);
    for(int i = 0; i < c->row * c->col; ++i){
        for(int j = 0; j < qnt; ++j){
            c->datarow[i] += a[j]->datarow[i];
            c->datacol[i] += a[j]->datacol[i];
        }
    }
    /*printf("SOMANDO!!!\n");
    for(int i = 0; i < qnt; ++i){
        m_print(a[i]);
    }
    printf("RESULTADO\n");
    m_print(c);
    printf("\n");*/
    return c;
}

matrix* th_plus(matrix** a, int qnt){
    printf("SOMANDO!!!\n");
    for(int i = 0; i < qnt; ++i){
        m_print(a[i]);
    }
    int row = a[0]->row;
    int col = a[0]->col;
    matrix* c = m_alloc(row, col);
    m_reset(c, 0);
    for(int j = 0; j < qnt; ++j){
        printf("###somando tudo da matrix %d\n", j);
        for(int i = 0; i < row * col; ++i){
            c->datarow[i] += a[j]->datarow[i];
            c->datacol[i] += a[j]->datacol[i];
            printf("%d ", c->datarow[i]);
        }
        printf("\n");
    }
    printf("RESULTADO!\n");
    m_print(c);
    return NULL;
}

matrix* m_mult(matrix* a, matrix* b){
    if(a->col != b->row){
        return NULL;
    }
    int aux;
    matrix* c = m_alloc(a->row, b->col);
    for(int i = 0; i < c->row; ++i){
        for(int j = 0; j < c->col; ++j){
            aux = 0;
            for(int k = 0; k < a->col; ++k){
                aux += a->datarow[k + (a->col * i)] * b->datacol[k + (b->row * j)];
            }
            c->datarow[j + (c->col * i)] = aux;
            c->datacol[i + (c->row * j)] = aux;
        }
    }
    return c;
}

matrix* b_mult(matrix** a, matrix** b, int qnt){
    matrix** c = (matrix**) malloc( qnt*sizeof(matrix*));
    for(int i=0; i < qnt; ++i){
        c[i] = m_mult(a[i], b[i]);
    }
    matrix* d = b_plus(c, qnt);
    for(int i = 0; i < qnt; ++i)
        m_free(c[i]);
    free(c);
    return d;
}

void* th_mult(void* args){
    th_args* dados = (th_args*) args;
    //printf("THREAD!!");
    dados->b_c = b_mult(dados->b_a, dados->b_b, dados->qnt);
    return NULL;
}