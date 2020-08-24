#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "tools.h"
#include "matriz.h"
#include "operacoes.h"
#include "operacoes_mpi.h"
#define RUNS 4

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int main(int argc, char *argv[]) {

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	// DECLARAÇÃO de VARIÁVEIS
	//mymatriz *mat_a, *mat_b, *mat_c;
	//mymatriz **mat_mult = NULL; //, **mat_b_mult = NULL, **mat_mpi_mult = NULL, **mat_mpi_b_mult = NULL;
    matrix *mat_a, *mat_b;
    matrix **blocos_a, **blocos_b;
    matrix **mat_mult = NULL, **mat_b_mult = NULL, **mat_mpi_mult = NULL, **mat_mpi_b_mult = NULL;
    //matriz_bloco_t **blocos_a = NULL, **blocos_b = NULL;

    int rank, size;
	char filename[100];
	FILE* f;
	int L1, C1, L2, C2, n_blocos;
	double start_time, end_time;
    double time_mult[RUNS], time_b_mult[RUNS], time_mpi_mult[RUNS], time_mpi_b_mult[RUNS];
    // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

    //MPI SETUP
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //MATRIX_SETUP
    mat_a = ( matrix* ) malloc( sizeof( matrix ) );
    mat_b = ( matrix* ) malloc( sizeof( matrix ) );
    //mat_c = ( matrix* ) malloc( sizeof( matrix ) );

    if(!rank) { //MASTER THREAD ONLY
        mat_mult = ( matrix** ) malloc ( sizeof( matrix* ) * RUNS);
        mat_b_mult = ( matrix** ) malloc ( sizeof( matrix* ) * RUNS);
        mat_mpi_mult = ( matrix** ) malloc ( sizeof( matrix* ) * RUNS);
        mat_mpi_b_mult = ( matrix** ) malloc ( sizeof( matrix* ) * RUNS);

        if (argc != 6){
            printf ("ERRO: Numero de parametros %s <linhas_matriz_a> <colunas_matriz_a> <linhas_matriz_b> <colunas_matriz_b> <nro_blocos>\n", argv[0]);
            exit (1);
        }

        L1 = atoi(argv[1]);
        C1 = atoi(argv[2]);
        L2 = atoi(argv[3]);
        C2 = atoi(argv[4]);
        n_blocos = atoi(argv[5]);

        // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
        //                Matriz A 
        mat_a = m_alloc(L1, C1);
        m_reset(mat_a, -1);
        f = fopen("A.map","w");
        m_file(mat_a, f);
        fclose(f);
        
        // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

        // %%%%%%%%%%%%%%%%%%%% %%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
        //               Leitura da Matriz B (arquivo)
        mat_b = m_alloc(L2, C2);
        m_reset(mat_b, -1);
        f = fopen("B.map","w");
        m_file(mat_b, f);
        fclose(f);
        // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

        if(n_blocos > mat_a->col) {
			printf("ERROR: número de blocos excede o valor máximo\n");
            exit(1);
		}

		if(mat_a->col != mat_b->row) {
			printf("ERROR: Matrizes Incompatíveis\n");
			exit(1);
		}

        // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
        //               Multiplicação Sequencial

        //RUN times 
        for(int i = 0; i < RUNS ; ++i){
            printf("\n ##### multiplicação sequencial %d #####\n", i);

            start_time = wtime();
            mat_mult[i] = m_mult(mat_a, mat_b);
            end_time = wtime();

            //mimprimir(mat_mult[ii]);
            printf("\t Tempo de execução: %f\n", end_time - start_time); 
            time_mult[i] = end_time - start_time;
            sprintf(filename, "mult_seq%d.result", i);
            f = fopen(filename,"w");
            m_file(mat_mult[i], f);
            fclose(f);
        }
        // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

        // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
        //              Comparação dos resultados

        printf("\n ##### Comparando resultados das execuções #####\n");
        for (int i=1; i < RUNS; ++i) {
            printf("mat_seq0 && mat_seq%d\t", i);
            if(m_compare (mat_mult[0], mat_mult[i])){
                exit(1);
            }
        }

        // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
        //
        //
        //
        // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//                 Multiplicação em bloco


        for(int i = 0; i < RUNS ; ++i) {
		    printf("\n ##### multiplicação em Blocos %d #####\n", i);
            //matrix* m_alloc(int row, int col);
            //matrix** divide(matrix* m, int orientacao, int qnt);
            //mat_c = m_alloc(C1, L2);
            //printf("TUDO OK 1\n");
            start_time = wtime();
            blocos_a = divide (mat_a, 1, n_blocos);
            blocos_b = divide (mat_b, 0, n_blocos);
            //printf("TUDO OK 2\n"); 
            mat_b_mult[i] = b_mult_diferente(blocos_a, blocos_b, n_blocos);
            end_time = wtime();
            //printf("TUDO OK 3\n");
            printf("\t Tempo de execução: %f\n", end_time - start_time); 
            time_b_mult[i] = end_time - start_time;

            sprintf(filename, "mult_bloco_seq%d.result", i);
            f = fopen(filename,"w");
            m_file(mat_b_mult[i], f);
            fclose(f);
        }
        // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

        // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
        //              Comparação dos resultados

        printf("\n ##### Comparando resultados das execuções #####\n");
        for (int i=1; i < RUNS; ++i) {
            printf("mat_bloco_seq0 && mat_bloco_seq%d\t", i);
            if(m_compare (mat_b_mult[0], mat_b_mult[i])){
                exit(1);
            }
        }
        // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
        
    }

    // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
    //                 Multiplicação MPI

    for (int i = 0; i < RUNS ; ++i) {
        if(!rank){
            printf("\n ##### multiplicação c/ MPI %d #####\n", i);
            start_time = wtime();
            mat_mpi_mult[i] = mpi_mult(mat_a, mat_b, rank, size);
            end_time = wtime();

            printf("\t Tempo de execução: %f\n", end_time - start_time); 
            time_mpi_mult[i] = end_time - start_time;
            
            sprintf(filename, "mult_mpi%d.result", i);
            f = fopen(filename,"w");
            m_file(mat_mpi_mult[i], f);
            fclose(f);
        }
        else{
            mpi_mult(mat_a, mat_b, rank, size);
        }
    }
    // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
    if(!rank){
        // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
        //              Comparação dos resultados

        printf("\n ##### Comparando resultados das execuções #####\n");
        for (int i=1; i < RUNS; ++i) {
            printf("mult_mpi0 && multt_mpi%d\t", i);
            if(m_compare (mat_mpi_mult[0], mat_mpi_mult[i])){
                exit(1);
            }
        }
        // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
    }

    // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
    //                 Multiplicação MPI Bloco

    for (int i = 0; i < RUNS ; ++i) {
        if(!rank){
            printf("\n ##### multiplicação c/ MPI em Bloco %d #####\n", i);
            start_time = wtime();
            mat_mpi_b_mult[i] = mpi_mult_bloco(mat_a, mat_b, rank, size);
            end_time = wtime();

            printf("\t Tempo de execução: %f\n", end_time - start_time); 
            time_mpi_b_mult[i] = end_time - start_time;

            sprintf(filename, "mult_mpi_b%d.result", i);
            f = fopen(filename,"w");
            m_file(mat_mpi_b_mult[i], f);
            fclose(f);
        }
        else{
            mpi_mult_bloco(mat_a, mat_b, rank, size);
        }
    }
    // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

    if(!rank){
        // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
        //              Comparação dos resultados

        printf("\n ##### Comparando resultados das execuções #####\n");
        for (int i=1; i < RUNS; ++i) {
            printf("mult_mpi_b0 && mult_mpi_b%d\t", i);
            if(m_compare (mat_mpi_b_mult[0], mat_mpi_b_mult[i])){
                exit(1);
            }
        }
        // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
    }

    if(!rank){
        // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
        //              Comparação dos resultados e tempos

        printf("\n ##### Comparação dos resultados da Multiplicação de mult_seq c/ mult_bloco_seq #####\n");
        printf("[mult_seq0 vs mult_bloco_seq0]\t");
        m_compare (mat_mult[0], mat_b_mult[0]);

        printf("\n ##### Comparação dos resultados da Multiplicação de mult_seq c/ mult_imp #####\n");
        printf("[mult_seq0 vs mult_mpi0]\t");
        m_compare (mat_mult[0], mat_mpi_mult[0]);
        printf("\n ##### Comparação dos resultados da Multiplicação de mult_seq c/ mult_imp_b #####\n");
        printf("[mult_seq0 vs mult_mpi_b0]\t");
        m_compare (mat_mult[0], mat_mpi_b_mult[0]);
        // ########################################################

        printf("\n ##### Tempo Médio Matriz Sequencial #####\n");
        double soma = 0;
        for(int i = 0; i < RUNS; i++){
            soma = soma + time_mult[i];
        }
        soma = soma / RUNS;
        printf("\tRuntime: %f\n", soma);

        printf("\n ##### Tempo Médio Matriz Sequencial em Bloco #####\n");
        double soma_b = 0;
        for(int i = 0; i < RUNS; i++){
            soma_b = soma_b + time_b_mult[i];
        }
        soma_b = soma_b / RUNS;
        printf("\tRuntime: %f\n", soma_b);

        printf("\n ##### Tempo Médio Matriz Sequencial OpenMPI #####\n");
        double somaMPI = 0;
        for(int i = 0; i < RUNS; i++){
            somaMPI = somaMPI + time_mpi_mult[i];
        }
        somaMPI = somaMPI / RUNS;
        printf("\tRuntime: %f\n", somaMPI);

        printf("\n ##### Tempo Médio Matriz Sequencial em Bloco OpenMPI #####\n");
        double somaMPI_b = 0;
        for(int i = 0; i < RUNS; i++){
            somaMPI_b = somaMPI_b + time_mpi_b_mult[i];
        }
        somaMPI_b = somaMPI_b / RUNS;
        printf("\tRuntime: %f\n", somaMPI_b);

        // ########################################################

        double speedup_seq = soma / somaMPI;
        double speedup_bloco = soma_b / somaMPI_b;
        printf("\n##### Speedups #####\n");
        printf("Sequencial:\t%f\nBloco:\t%f\n", speedup_seq, speedup_bloco);

    }



    if(!rank){
        for (int i=0; i < RUNS; ++i) {
            m_free (mat_mult[i]);
            m_free (mat_b_mult[i]);
            m_free (mat_mpi_mult[i]);
            m_free (mat_mpi_b_mult[i]);
        }
        m_free(mat_a);
        m_free(mat_b);
        free(mat_mult);
        free(mat_b_mult);
        free(mat_mpi_mult);
        free(mat_mpi_b_mult);
    }
    MPI_Finalize();

	

	return 0;
}
