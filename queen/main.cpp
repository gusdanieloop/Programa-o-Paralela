#include <iostream>
#include <algorithm>
#include <cmath>

//baseando-se nos dados informados, verifica se cabe todos os cavaleiros, e se cabe alguma rainha
int enough_space(const int knights, const int n){
    int res = 0;
    if(n % 2 == 0){
        if(2 * pow(n/2, 2) >= knights){
            res++; //cabe todos os cavaleiros
            if( pow(n/2, 2) >= knights){
                res++; //cabe rainhas
            }
        }
    }
    else{
        if(pow((n+1)/2, 2) + pow(floor(n/2), 2) >= knights){
            res++; //cabe todos os cavaleiros
            if(pow(floor(n/2),2) >= knights){
                res++; //cabe rainhas
            }
        }
    }
    return res;
}

//limite por linha
int limit(const int n){
    return floor(n/2);
}

//função que cheguei ao analisar o comportamento da quantidade ideal de diagonais para x cavaleiros, só que tem formato de um triangulo, tem que dividir o quadrado no meio
int* qtd(int knights){
    int* cont = ( int*) malloc(sizeof(int) * (knights+1));
    for(int i=0; i <= knights; i++){
        cont[i] = ceil(sqrt(i));
    }
    return cont;
}

int qtd_ate_aqui(int diagonais, int linha){
    if(linha == 0){
        return 0;
    }
    int soma = 0;
    for(int i = 0; i < linha; ++i){
        soma += (diagonais - ceil(i/2));
    }
    return soma;
}


void create_board_no_queens(int** board, const int n, int knights){
    for(int i = 0; i < n; ++i){
        if(i%2 == 0){
            for(int j = 0; j < n; j+=2){
                board[i][j] = 1;
                knights--;
                if(!knights)
                    return;
            }
        }
        else{
            for(int j = 1; j < n; j+=2){
                board[i][j] = 1;
                knights--;
                if(!knights)
                    return;
            }
        }
    }
}

void lock_places_board(int** board, const int n, const int i, const int j){
    board[i+2][j+1] = -1;
    for(int ii = 0; ii < n; ++ii){
        if(board[ii][j] == 0){
            board[ii][j] = -1;
        }
        if(board[i][ii] == 0){
            board[i][ii] = -1;
        }
    }
    int ii = i;
    int jj = j;
    while(ii < n && jj < n){
        if(board[ii][jj] == 0){
            board[ii][jj] = -1;
        }
        ii++;
        jj++;
    }
}

void fill(int** board, const int n, const int m){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            if(board[i][j] == 0)
                board[]
        }
    }
}

void create_board(int** board, const int n, int knights){
    int* qt = qtd(knights);
    int aux = *(qt+knights);
    int posx, posy;
    int feitos;
    int xcolunas;
    //colocar omp aqui
    for(int i = 0; i < aux; ++i){
        feitos = i*i;
        xcolunas = (2*i)+1;
        if(feitos + xcolunas > knights){
            xcolunas = knights - feitos;
        }
        for(int j = 0; j < xcolunas; ++j){
            board[j][(2*i)-j] = 1;
        }
    }

    if(aux*aux > knights){
        fill(board, aux, aux-1);
    }
    else{
        fill(board, aux, aux);
    }
    //int aux2;
    //int soma;
    /*for(int i = 0; i < n; i++){
        aux2 = std::max(0, (int) (aux - ceil(i/2.0)));
        std::cout << "linha " << i << " - " << aux2 << " diagonais" << std::endl;
        //soma = knights - qtd_ate_aqui(aux, i);
        std::cout << "linha " << i << " " << qtd_ate_aqui(aux, i) << std::endl;
        //if(soma < 1){
        //    break;
        //}
        if(i%2 == 0){
            for(int j = 0; j < n && j < (aux2*2); j+=2){
                board[i][j] = 1;
                //soma--;
                //if(soma < 1){
                //    break;
                //}
                //lock_places_board(board, n, i, j);
            }
            //aux2--;
        }
        else{
            for(int j = 1; j < n && j < (aux2*2); j+=2){
                board[i][j] = 1;
                //soma--;
                //if(soma < 1){
                //    break;
                //}
                //lock_places_board(board, n, i, j);
            }
        }
    }*/
}



void print_board(int** board, const int n){
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            std::cout << board[i][j] << ";\t";
        }
        std::cout << std::endl;
    }
}

int count_free_spots(int** board, const int n){
    int cont = 0;
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            if(board[i][j] == 0)
                cont++;
        }
    }
    return cont;
}

//void lock_places_queen

void check_queens(int** board, const int n){
    int m;
    int** board2 = (int**)malloc(sizeof(int*) * n);
    for(int i = 0; i < n; i++){
        board2[i] = (int*)malloc(sizeof(int) * n);
    }
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            if(board[i][j] == 0){
                board2 = board;
                board2[i][j] = 2;
                
            }
        }
    }
}

int main(int argc, char **argv) {
    int rank, size;

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

    int can_do = enough_space(knights, n);
    if(!can_do){
        std::cout << "Não é possível" << std::endl;
        return 0;
    }
    else if(can_do == 1){   
        std::cout << "Nenhuma rainha vai ter, mas da pra fazer" << std::endl; //passa do meio...então nem preciso fazer verificação nenhuma :D
        create_board_no_queens(board, n, knights);
    }
    else if(can_do == 2){
        std::cout << "Da pra fazer e vai ter rainhas!" << std::endl;
        create_board(board, n, knights);
    }
    print_board(board,n);
    return 0;
}