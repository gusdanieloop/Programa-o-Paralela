#include <iostream>
#include <algorithm>
#include <cmath>
#include <omp.h>
#include <limits>

int empty_spots(int** board, const int n){
    int soma = 0;
    #pragma omp parallel for shared(board) reduction(+:soma)
    for(int i = 0; i < n; ++i){
        #pragma omp parallel for shared(board) reduction(+:soma)
        for(int j = 0; j < n; ++j){
            if(board[i][j] == 0){
                soma++;
            }
        }
    }
    #pragma omp barrier
    return soma;
}

void print_board(int** board, const int n){
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            if(board[i][j] == 1)
                std::cout << "K;\t";
            else if(board[i][j] == 2)
                std::cout << "Q;\t";
            //else if(board[i][j] == -1)
            //    std::cout << "-;\t";
            else
                std::cout << (i*n)+j+1 << ";\t";
        }
        std::cout << std::endl;
    }
}

void print_positions(int** board, const int n){
    for (int i = 0; i < n; ++i){
        for (int j = 0; j < n; ++j){
            if(board[i][j] == 1){
                std::cout << "K" << (i*n)+j+1 << ";";
            }
            if(board[i][j] == 2){
                std::cout << "Q" << (i*n)+j+1 << ";";
            }
        }
    }
    std::cout << std::endl;
}

int menor_quadrado(const int knights){
    int aux = ceil(sqrt(knights*2.0));
    if(ceil((aux-1)*(aux-1)/2.0) >= knights){
        return (aux-1);
    }
    return aux;
}

int check_menor(const int knights, const int menor){
    int aux;
    if(menor > 2){
        if(menor%2 == 0){
            aux = ((menor-2)/2) + 1;
            if((aux*aux) + aux > knights){
                return menor-1;
            }
            else{
                return menor;
            }
        }
        else{
            aux = ((menor-1)/2) + 1;
            if((aux*aux) > knights){
                return menor-1;
            }
            else{
                return menor;
            }
        }
    }else{
        return knights;
    }
}

int ja_foi_upper(const int diagonais, const int min){
    if(min==1){
        return(diagonais*diagonais);
    }
    return (diagonais*diagonais)+diagonais;
}


int ja_foi_under(const int diagonais, const int min){
    int aux, remover;
    aux = ceil(min/2.0); //total possível de diagonais
    remover = aux - diagonais; //o que tem que tirar do total
    int total, total2;
    if(min%2!=0){
        total = aux*aux;
        total2 = remover*remover;
    }
    else{
        total = (aux*aux) + aux;
        total2 = (remover*remover) + remover;
    }
    return total-total2;
    
}

void create_board(int** board, const int menor, int knights){
    int col;
    int min;
    if(menor>2){
        if(menor%2 == 0){
            col = menor-2;
            min = 2;
        }
        else{
            col = menor-1;
            min = 1;
        }
    }
    else{
        col = 0;
        if(menor>0){
            min = menor;
        }
        else{
            min = 1;
        }
    }
    int foi;
    int falta;
    //upper
    int k;
    #pragma omp barrier
    #pragma omp parallel for private(foi, falta, k) shared(board)
    for(int i = col; i >= 0; i-=2){
        k = (col - i) /2.0;
        foi = ja_foi_upper(k, min);
        falta = knights - foi;
        #pragma omp parallel for shared(board)
        for(int j = 0; j < std::min(min + (2*k), falta); ++j){
            board[j][j+i] = 1;
        }
    }
    knights = knights - ja_foi_upper((col/2) + 1, min);
    if(knights > 0){
        //under
        min = menor - 2;
        #pragma omp barrier
        #pragma omp parallel for private(foi, falta, k) shared(board) 
        for(int i = 2; i <= col; i+=2){
            k = (i/2) - 1;
            foi = ja_foi_under(k, min);
            falta = knights - foi;
            #pragma omp parallel for shared(board)
            for(int j = 0; j < std::min(min - (2*k), falta); ++j){
                board[i+j][j] = 1;
            }
        }
        #pragma omp barrier
    }
}

void fill(int** board, const int menorx, const int menory, const int n){
    #pragma omp barrier
    #pragma omp parallel for shared(board)
    for(int i = 0; i < n; ++i){
        #pragma omp parallel for shared(board)
        for(int j = 0; j < menory; ++j){
            if(board[i][j] == 0){
                board[i][j] = -1;
            }
            else if(board[i][j] == 1){
                board[i+1][j+2] = -1;
                board[i+2][j+1] = -1;
            }
        }
        #pragma omp parallel for shared(board)
        for(int j = 0; j < menorx; ++j){
            if(board[j][i] == 0){
                board[j][i] = -1;
            }
            else if(board[j][i] == 1){
                board[j+1][i+2] = -1;
                board[j+2][i+1] = -1;
            }
        }
    }
    #pragma omp barrier
    #pragma omp parallel for shared(board)
    for(int i = 0; i < menory; i+=2){
        if(board[i][0] == 1){
            #pragma omp parallel for shared(board)
            for(int j = 0; j < n-i; ++j){
                if(board[i+j][j] == 0){
                    board[i+j][j] = -1;
                }
            }
        }
        if(board[0][i] == 1){
            #pragma omp parallel for shared(board)
            for(int j = 0; j < n-i; ++j){
                if(board[j][j+i] == 0){
                    board[j][j+i] = -1;
                }
            }
        }
    }
}

int check_spot(int**board, const int menorx, const int menory, const int n, const int i, const int j){
    int soma = 0;
    int mini, mfim;
    int startx, starty;
    #pragma omp parallel for reduction(+:soma)
    for(int ii = menory; ii < n; ++ii){
        if(board[i][ii] == 0) {
            soma++;
        }
    }
    #pragma omp parallel for reduction(+:soma)
    for(int ii = menorx; ii < n; ++ii){
        if(board[ii][j] == 0) {
            soma++;
        }
    }
    mini = std::min(i-menorx, j-menory);
    startx = i - mini;
    starty = j - mini;
    mfim = std::min(n-1-startx, n-1-starty);
    #pragma omp parallel for reduction(+:soma)
    for(int k = 0; k <= mfim; ++k) {
        if(board[startx+k][starty+k] == 0){
            soma++;
        }
    }
    mini = std::min(i-menorx, n-1-j);
    startx = i - mini;
    starty = j + mini;
    mfim = std::min(n - 1 - startx, starty - menory);
    #pragma omp parallel for reduction(+:soma)
    for(int k = 0; k <= mfim; ++k) {
        if(board[startx+k][starty-k] == 0){
            soma++;
        }
    }
    return soma-4;
}

void fill_queen(int** board, const int menorx, const int menory, const int n, const int i, const int j){
    #pragma omp parallel for shared(board)
    for(int jj = menory; jj < n; ++jj){
        if(board[i][jj] == 0)
            board[i][jj] = -1;
    }
    #pragma omp parallel for shared(board)
    for(int jj = menorx; jj < n; ++jj){
        if(board[jj][j] == 0)
            board[jj][j] = -1;
    }
    int mini = std::min(i-menorx, j-menory);
    int startx = i - mini;
    int starty = j - mini;
    int mfim = std::min(n-1-startx, n-1-starty);
    #pragma omp parallel for shared(board)
    for(int k = 1; k <= mfim; ++k) {
        if(board[startx+k][starty+k] == 0){
            board[startx+k][starty+k] = -1;
        }
    }

    mini = std::min(i-menorx, n-1-j);
    startx = i - mini;
    starty = j + mini;
    mfim = std::min(n-1 - startx, starty - menory);
    #pragma omp parallel for shared(board)
    for(int k = 1; k <= mfim; ++k) {
        if(board[startx+k][starty-k] == 0){
            board[startx+k][starty-k] = -1;
        }
    }
}

void check_queens(int** board, const int menorx, const int menory, const int n){
    int melhor;
    int melhory, melhorx;
    int aux = 0;
    int cont = empty_spots(board, n);
    omp_set_nested(1);
    while(cont > 0){
        melhor = std::numeric_limits<int>::max();
        #pragma omp parallel for shared(melhor, melhorx, melhory, board) private(aux) 
        for(int i = menorx; i < n; ++i){
            #pragma omp parallel for shared(melhor, melhorx, melhory, board) private(aux)
            for(int j = menory; j < n; ++j){
                if(board[i][j] == 0){
                    aux = check_spot(board, menorx, menory, n, i, j);
                    #pragma omp critical
                    {
                        if(aux < melhor){
                            melhor = aux;
                            melhorx = i;
                            melhory = j;
                        }
                    }
                }
            }
        }
        #pragma omp barrier
        //std::cout << "feito! " << (melhorx*n)+melhory+1 << std::endl;
        board[melhorx][melhory] = 2;
        fill_queen(board, menorx, menory, n, melhorx, melhory);
        cont = empty_spots(board, n);
        #pragma omp barrier
    }
}



int main(int argc, char **argv) {

    if (argc < 3){
        printf ("./main <tam_tabuleiro> <qtd_knights>\n");
        return -1;
    }
    
    //pegando parametros do console
    int n = atoi(argv[1]);
    int knights = atoi(argv[2]);
    
    //criando tabuleiro n x n
    int** board = (int**) malloc(sizeof(int*) * n);
    for(int i = 0; i < n; ++i) {
        board[i] = (int*) malloc(sizeof(int) * n);
        for(int j = 0; j < n; ++j){
            board[i][j] = 0;
        }
    }

    int menor = menor_quadrado(knights);
    int menor2 = check_menor(knights, menor);
    if(menor > n){
        std::cout << "Não é possível" << std::endl;
    }
    else{
        create_board(board, menor, knights);
        //std::cout << "ANTES DO FILL" << std::endl;
        //print_board(board, n);
        fill(board, menor2, menor, n);
        //std::cout << "DEPOIS DO FILL" << std::endl;
        //print_board(board, n);
        check_queens(board, menor2, menor, n);
        //std::cout << "DEPOIS DAS RAINHAS" << std::endl;
        print_board(board, n);
        std::cout << std::endl;
        print_positions(board, n);

    }
    return 0;
}