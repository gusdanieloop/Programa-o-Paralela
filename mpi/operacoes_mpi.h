#ifndef OPERATIONS_MPI_HPP
#define OPERATIONS_MPI_HPP

#include "matriz.h"

matrix* mpi_mult(matrix* a, matrix* b, int rank, int size);
matrix* mpi_mult_bloco(matrix* a, matrix* b, int rank, int size);
#endif