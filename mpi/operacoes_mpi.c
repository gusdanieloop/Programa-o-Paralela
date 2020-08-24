#include <mpi.h>
#include <math.h>
#include <unistd.h>

#include "matriz.h"
#include "tools.h"
#include "operacoes.h"
#include "operacoes_mpi.h"

matrix* mpi_mult(matrix* a, matrix* b, int rank, int size) {
    matrix* res = m_alloc(a->row, b->col);
    matrix* aux = (matrix*) malloc(sizeof(matrix));

    int* info = (int*) malloc(4 * sizeof( int) );
    if(!rank){
        info[0] = ceil(a->row/size);
        info[1] = a->col;
        info[2] = b->row;
        info[3] = b->col;
    }

    MPI_Bcast(info, 4, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank){
        b = m_alloc(info[2], info[3]);
    }
    aux = m_alloc(info[0], info[1]);

    //MPI_Bcast(b->datarow, b->row*b->col, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(b->datacol, b->row*b->col, MPI_INT, 0, MPI_COMM_WORLD);


    MPI_Scatter(a->datarow, aux->row*aux->col, MPI_INT, aux->datarow, aux->row*aux->col, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Scatter(a->datacol, aux->row*aux->col, MPI_INT, aux->datacol, aux->row*aux->col, MPI_INT, 0, MPI_COMM_WORLD);

    matrix* aux2 = m_mult(aux, b);

    MPI_Gather(aux2->datarow, aux2->row*aux2->col, MPI_INT, res->datarow, aux2->row*aux2->col, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(aux2->datacol, aux2->row*aux2->col, MPI_INT, res->datacol, aux2->row*aux2->col, MPI_INT, 0, MPI_COMM_WORLD);

    m_free(aux);
    m_free(aux2);
    free(info);
    return res;
}

matrix* mpi_mult_bloco(matrix* a, matrix* b, int rank, int size) {

    matrix* res = NULL;

    matrix* auxA = (matrix*) malloc(sizeof(matrix));
    matrix* auxB = (matrix*) malloc(sizeof(matrix));

    int* info = (int*) malloc(4 * sizeof( int) );
    if(!rank){
        info[0] = a->row;
        info[1] = a->col;
        info[2] = b->row;
        info[3] = b->col;
    }

    MPI_Bcast(info, 4, MPI_INT, 0, MPI_COMM_WORLD);

        
    res = m_alloc(info[0], info[3]);
    m_reset(res, 0);
    
    
    int qnt = ceil(info[1] / size);

    auxA = m_alloc(info[0], qnt); //divisao na vertical
    auxB = m_alloc(qnt, info[3]); //divisao na horizontal

    MPI_Scatter(a->datacol, auxA->row*auxA->col, MPI_INT, auxA->datacol, auxA->row*auxA->col, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Scatter(a->datacol, auxA->row*auxA->col, MPI_INT, auxA->datacol, auxA->row*auxA->col, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(b->datarow, auxB->row*auxB->col, MPI_INT, auxB->datarow, auxB->row*auxB->col, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Scatter(b->datacol, auxB->row*auxB->col, MPI_INT, auxB->datacol, auxB->row*auxB->col, MPI_INT, 0, MPI_COMM_WORLD);
    auxA->datarow = transpose(auxA->datacol, auxA->col, auxA->row);
    auxB->datacol = transpose(auxB->datarow, auxB->row, auxB->col);
    
    
    matrix* aux = m_mult(auxA, auxB);
    
    MPI_Reduce(aux->datarow, res->datarow, info[0] * info[3], MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(aux->datacol, res->datacol, info[0] * info[3], MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


    m_free(aux);
    m_free(auxA);
    m_free(auxB);
    free(info);

    return res;
}