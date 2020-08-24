#include <stdio.h>
#include <stdlib.h>

#include "toolsv3.h"
#include "matrizv3.h"
#include "matriz-operacoesv3.h"

int main( int argc, char* argv[]) {
    int** matriz = ( int**) malloc( 3 * sizeof( int*));
    for( int i = 0; i < 3; ++i) {
        matriz[i] = ( int*) malloc( 4 * sizeof( int));
    }
    matriz[0][0] = 1;
    matriz[0][1] = 2;
    matriz[0][2] = 1;
    matriz[0][3] = 2;
    matriz[1][0] = 2;
    matriz[1][1] = 1;
    matriz[1][2] = 3;
    matriz[1][3] = 1;
    matriz[2][0] = 2;
    matriz[2][1] = 1;
    matriz[2][2] = 2;
    matriz[2][3] = 1;

    int** matriz2 = ( int**) malloc( 4 * sizeof( int*));
    for( int i = 0; i < 4; ++i) {
        matriz2[i] = ( int*) malloc( 3 * sizeof( int));
    }
    matriz2[0][0] = 3;
    matriz2[0][1] = 4;
    matriz2[0][2] = 3;
    matriz2[1][0] = 1;
    matriz2[1][1] = 1;
    matriz2[1][2] = 2;
    matriz2[2][0] = 4;
    matriz2[2][1] = 1;
    matriz2[2][2] = 4;
    matriz2[3][0] = 2;
    matriz2[3][1] = 0;
    matriz2[3][2] = 1;


    int linhas = 3;
    int colunas = 4;
    int orientacao = 0; //vertical
    int partes = 2;
    matriz_bloco_t** blocos_a = particionar_matriz(matriz, linhas, colunas, orientacao, partes);
    linhas = 4;
    colunas = 3;
    orientacao = 1; //horizontal
    matriz_bloco_t** blocos_b = particionar_matriz(matriz2, linhas, colunas, orientacao, partes);
    linhas = 3;
    matriz_bloco_t** blocos_c = csubmatrizv2(linhas, colunas, partes);
    for( int i = 0; i < partes; ++i) {
        printf("MULTIPLICAÇÃO PARTE %d\n", i+1);
        mmsubmatriz(blocos_a[i], blocos_b[i], blocos_c[i]);
    }

    return 0;
}