#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP

#include "matriz.h"

matrix* m_plus(matrix* a, matrix* b);
matrix* b_plus(matrix** a, int qnt);
matrix* th_plus(matrix** a, int qnt);
matrix* m_mult(matrix* a, matrix* b);
matrix* b_mult(matrix** a, matrix** b, int qnt);
void* th_mult(void* args);
#endif