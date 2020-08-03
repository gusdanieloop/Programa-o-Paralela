#include "matriz.h"

void init_random(){
    srand((unsigned) time(NULL));
}

matrix* m_alloc(int row, int col){
    int* data = ( int*) malloc( row * col * sizeof( int) );
    int* t_data = ( int*) malloc( row * col * sizeof( int) );
    matrix* m = ( matrix*) malloc( sizeof( matrix));
    m->datarow = data;
    m->datacol = t_data;
    m->row = row;
    m->col = col;
    return m;
}



matrix** divide(matrix* m, int orientacao, int qnt){
    //orientacao: 0 => horizontal | 1 => vertical
    int r_beg = 0, c_beg = 0;
    int aux;
    matrix** b = ( matrix**) malloc( sizeof( matrix*) * qnt);
    for(int i = 0; i < qnt; ++i) {
        b[i] = (matrix*) malloc(sizeof(matrix));
    }
    if(orientacao){
        int size = (int) m->col / qnt;
        int remaining = (int) m->col % qnt;
        for(int i = 0; i < qnt; ++i) {
            c_beg = i * size;
            b[i]->row = m->row;
            if((i + 1) == qnt){
                b[i]->col = size + remaining; 
            }
            else{
                b[i]->col = size;
            }
            b[i]->datarow = (int*) malloc(b[i]->row * b[i]->col * sizeof(int));
            b[i]->datacol = (int*) malloc(b[i]->col * b[i]->row * sizeof(int));
            for(int j = 0; j < b[i]->row; ++j ) {
                for(int k = 0; k < b[i]->col; ++k) {
                    aux = m->datarow[(k + c_beg) + (m->col * j)];
                    b[i]->datarow[k + (b[i]->col * j)] = aux;
                    b[i]->datacol[j + (b[i]->row * k)] = aux;
                }
            }
        }
    }
    else {
        int size = (int) m->row / qnt;
        int remaining = (int) m->row % qnt;
        int i;
        for( i = 0; i < qnt; ++i) {
            r_beg = i * size;
            b[i]->col = m->col;
            if(i + 1 == qnt){
                b[i]->row = size + remaining;
            }
            else{
                b[i]->row = size;
            }
            b[i]->datarow = (int*) malloc(b[i]->row * b[i]->col * sizeof(int));
            b[i]->datacol = (int*) malloc(b[i]->col * b[i]->row * sizeof(int));
            for(int j = 0; j < b[i]->row; ++j ) {
                for(int k = 0; k < b[i]->col; ++k) {
                    aux = m->datarow[k + (m->col * (j + r_beg))];
                    b[i]->datarow[k + (b[i]->col * j)] = aux;
                    b[i]->datacol[j + (b[i]->row * k)] = aux;
                }
            }
        }
        
    }
    return b;
}


th_args** t_alloc(matrix* a, matrix* b, int qnt, int n_threads){
    matrix** b_a = divide(a, 1, qnt);
    matrix** b_b = divide(b, 0, qnt);

    int skip = qnt / n_threads;
    int tam = 0;
    th_args** args = (th_args**) malloc(n_threads * sizeof(th_args*));
    for(int i = 0; i < n_threads; ++i){
        args[i] = (th_args*) malloc ( sizeof( th_args));
        if((i + 1) == n_threads)
            tam = (qnt / n_threads) + (qnt%n_threads);
        else
            tam = (qnt / n_threads);
        args[i]->b_a = (matrix**) malloc ( tam * sizeof(matrix*));
        args[i]->b_b = (matrix**) malloc ( tam * sizeof(matrix*));
        args[i]->b_c = m_alloc(a->row, b->col);
        //m_reset(args[i]->b_c, 0);
        args[i]->qnt = tam;
        for(int j = 0; j < tam; ++j){
            args[i]->b_a[j] = b_a[j + (skip * i)];
            args[i]->b_b[j] = b_b[j + (skip * i)];
        }
    }
    return args;
}




void m_reset(matrix* m, int value){
    if(value == -1) {
        int aux;
        for(int i = 0; i < m->row; i++) {
            for(int j = 0; j < m->col; ++j){
                aux = rand() % 10;
                m->datarow[j + (m->col * i)] = aux;
                m->datacol[i + (m->row * j)] = aux;
            }
        }
    }
    else{
        for(int i = 0; i < m->row * m->col; ++i) {
            m->datarow[i] = m->datacol[i] = value;
        }
    }
}

void m_print(matrix* m){
    printf("Matriz:\n");
    for(int i = 0; i < m->row; i++) {
        for(int j = 0; j < m->col; j++) {
            printf("%d ", m->datarow[j + (m->col * i)]);
        }
        printf("\n");
    }
    /*printf("Transposta:\n");
    for(int i = 0; i < m->col; i++) {
        for(int j = 0; j < m->row; j++) {
            printf("%d ", m->datacol[j + (m->row * i)]);
        }
        printf("\n");
    }*/
}

void m_free(matrix* m){
    free(m->datarow);
    free(m->datacol);
    free(m);
}

void m_file(matrix* m, FILE* f){
    for (int i=0; i < m->row; i++) {
        for (int j=0; j < m->col; j++){
            fprintf(f,"%d ", m->datarow[j + (m->col * i)]);
        }
    fprintf(f,"\n");
  }
  return;
}