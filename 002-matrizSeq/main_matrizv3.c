#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "toolsv3.h"
#include "matrizv3.h"
#include "matriz-operacoesv3.h"

pthread_mutex_t lock;

#define NBLOCOS (int)4

int main(int argc, char* argv[]) {

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	// DECLARAÇÃO de VARIÁVEIS
	mymatriz mat_a, mat_b, mat_ParC, mat_SeqC;
	mymatriz** mat_mult;
	mymatriz* resultado;
    matriz_bloco_t** mat_BlA;
    matriz_bloco_t** mat_BlB;
    matriz_bloco_t** mat_BlC;
	param_t* args;
	param_bloco_t* args2;
	pthread_t *threads;
	char filename[100];
	FILE* fmat;
	int nr_line;
	int* vet_line = NULL;
	int N, M, La, Lb;
    int nro_tarefas;
	double media_seqC = 0.0, media_seqBlC = 0.0, media_ThreadC = 0.0, media_ThreadBlC = 0.0;
	double start_time, end_time;
	// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

	if (argc != 4){
		printf ("ERRO: Numero de parametros %s <matriz_a> <matriz_b> <nr_tarefas>\n", argv[0]);
		exit (1);
	}

	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    nro_tarefas = atoi(argv[3]);
    mat_mult = ( mymatriz**) malloc( sizeof( mymatriz*) * 6);
	for(int i = 0; i < 8; ++i ) {
        mat_mult[i] = (mymatriz*) malloc ( sizeof( mymatriz));
    }

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//                Leitura da Matriz A (arquivo)
	fmat = fopen(argv[1],"r");
	if (fmat == NULL) {
		printf("Error: Na abertura dos arquivos.");
		exit(1);
	}
	extrai_parametros_matriz(fmat, &N, &La, &vet_line, &nr_line);
	//return 1;
	mat_a.matriz = NULL;
	mat_a.lin = N;
	mat_a.col = La;
	if (malocar(&mat_a)) {
		printf ("ERROR: Out of memory\n");
	}
	filein_matriz (mat_a.matriz, N, La, fmat, vet_line, nr_line);
	free (vet_line);
	fclose(fmat);
	// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

	if (mat_a.lin < nro_tarefas || nro_tarefas == 0)
		nro_tarefas = mat_a.lin;

	threads = ( pthread_t*) malloc(sizeof(pthread_t) * nro_tarefas);
	args = ( param_t*) malloc( sizeof( param_t) * nro_tarefas);
	args2 = ( param_bloco_t*) malloc( sizeof( param_bloco_t) * nro_tarefas);
	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//               Leitura da Matriz B (arquivo)
	fmat = fopen(argv[2],"r");
	if (fmat == NULL) {
		printf("Error: Na abertura dos arquivos.");
		exit(1);
	}
	extrai_parametros_matriz(fmat, &Lb, &M, &vet_line, &nr_line);
	mat_b.matriz = NULL;
	mat_b.lin = Lb;
	mat_b.col = M;
	if (malocar(&mat_b)) {
		printf ("ERROR: Out of memory\n");
	}
	filein_matriz (mat_b.matriz, Lb, M, fmat, vet_line, nr_line);
	free (vet_line);
	fclose(fmat);
	// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
    
    // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%
    //                Criacao da Matriz C Paralelo
    mat_ParC.matriz = NULL;
    mat_ParC.lin = mat_a.lin;
    mat_ParC.col = mat_b.col;
	if( malocar( &mat_ParC)) {
		printf("ERROR: Out of memory\n");
	}
    // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

    // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%
    //                Criacao da Matriz C Sequencial
    mat_SeqC.matriz = NULL;
    mat_SeqC.lin = mat_a.col;
    mat_SeqC.col = mat_b.lin;
    if( malocar( &mat_SeqC)) {
        printf("ERROR: Out of memory\n");
    }
    // %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%
	// 				Criacao de parametro para threads
	for(int n = 0; n < nro_tarefas; ++n) {
		args[n].tid = n;
		args[n].ntasks = nro_tarefas;
		args[n].lini = n * (mat_a.lin / nro_tarefas);
		if( n == nro_tarefas - 1) 
			args[n].lfim = mat_a.lin;
		else
			args[n].lfim = args[n].lini + (mat_a.lin / nro_tarefas);
		args[n].mat_a = &mat_a;
		args[n].mat_b = &mat_b;
		args[n].mat_cPar = &mat_ParC;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%
	// 				Criacao de parametro para threads Bloco
	for(int n = 0; n < nro_tarefas; ++n) {
		args2[n].lock = &lock;
		args2[n].tid = n;
		args2[n].ntasks = nro_tarefas;
		args2[n].blini = n * (NBLOCOS / nro_tarefas);
		if( n == nro_tarefas - 1)
			args2[n].blfim = NBLOCOS;
		else
			args2[n].blfim = args2[n].blini + (NBLOCOS / nro_tarefas);
	}

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//               Multiplicação Sequencial
    for(int i = 0; i < 2; ++i) {
        printf("\n ### Multiplicação %d: Sequencial ### \n", i);
        start_time = wtime();
        mat_mult[i] = mmultiplicar(&mat_a, &mat_b, i);
        end_time = wtime();
        printf("\tRuntime: %f\n", end_time - start_time);
		media_seqC += (end_time - start_time);
        sprintf(filename, "mult_seq%d.result", i);
        fmat = fopen(filename, "w");
        fileout_matriz(mat_mult[i], fmat);
        fclose(fmat);
		printf("IMPRIMINDO MAT_MULT[%d]\n", i);
		mimprimir(mat_mult[i]);
    }
	media_seqC /= 2;
	// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//              Multiplicação Sequencial em Bloco
    for(int i = 2; i < 4; ++i) {
		mat_mult[i] = (mymatriz*) malloc (sizeof(mymatriz));
		mat_mult[i]->matriz = NULL;
		mat_mult[i]->lin = mat_a.lin;
		mat_mult[i]->col = mat_b.col;
		if( malocar( mat_mult[i])) {
			printf("ERROR: Out of memory\n");
		}
        printf("\n ### Multiplicação %d: Sequencial em Bloco  ### \n", i);
        start_time = wtime();
        mat_BlA = particionar_matriz(mat_a.matriz, mat_a.lin, mat_a.col, 0, NBLOCOS);
        mat_BlB = particionar_matriz(mat_b.matriz, mat_b.lin, mat_b.col, 1, NBLOCOS);
        mat_BlC = csubmatrizv2(mat_a.lin, mat_b.col, NBLOCOS);
        for(int j = 0; j < NBLOCOS; ++j) {
			//printf("BLOCO %d\n", j);
            mmsubmatriz(mat_BlA[j], mat_BlB[j], mat_BlC[j]);
			mat_mult[i] = msomar(mat_mult[i], mat_BlC[j]->matriz, 0);
        }
        end_time = wtime();
        printf("\tRuntime: %f\n", end_time - start_time);
		media_seqBlC += (end_time - start_time);
        sprintf(filename, "mult_seq_bloco%d.result", i);
        fmat = fopen(filename, "w");
        fileout_matriz(mat_mult[i], fmat);
        fclose(fmat);
		printf("IMPRIMINDO MAT_MULT[%d]\n", i);
		mimprimir(mat_mult[i]);
    }	
	media_seqBlC /= 2;
	// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//              Multiplicação Multithread
	for(int i = 4; i < 6; ++i) {
		mat_mult[i] = (mymatriz*) malloc (sizeof(mymatriz));
		mat_mult[i]->matriz = NULL;
		mat_mult[i]->lin = mat_a.lin;
		mat_mult[i]->col = mat_b.col;
		if( malocar( mat_mult[i])) {
			printf("ERROR: Out of memory\n");
		}
		printf("\n ### Multiplicação %d: Multithread ### \n", i);
		mzerar(&mat_ParC);
		start_time = wtime();
		for(int n = 0; n < nro_tarefas; ++n) {	
			pthread_create(&threads[n], NULL, multiplicarth, (void*) (args+n));
		}
		for(int n = 0; n < nro_tarefas; ++n) {
			pthread_join(threads[n], NULL);
		}
		end_time = wtime();
		mat_mult[i] = args[0].mat_cPar;
		printf("\tRuntime: %f\n", end_time - start_time);
		media_ThreadC += (end_time - start_time);
		sprintf(filename, "mult_thread%d.result", i);
		fmat = fopen(filename, "w");
		fileout_matriz(mat_mult[i], fmat);
		fclose(fmat);
		printf("IMPRIMINDO MAT_MULT[%d]\n", i);
		mimprimir(mat_mult[i]);
	}
	media_ThreadC /= 2;
	//linha_comeco = p->tid * (p->mat_a->lin / p->ntasks);

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//              Multiplicação Multithread Bloco
	for(int i = 6; i < 8; ++i) {
		resultado = (mymatriz*) malloc(sizeof(mymatriz));
		resultado->lin = mat_a.lin;
		resultado->col = mat_b.col;
		resultado->matriz = NULL;
		if (malocar(resultado)) {
			printf ("ERROR: Out of memory\n");
		}
		printf("\n ### Multiplicação %d: Multithread Bloco ### \n", i);
		start_time = wtime();
		matriz_bloco_t** mat_a_particionado = particionar_matriz(mat_a.matriz, mat_a.lin, mat_a.col, 0, NBLOCOS);
		matriz_bloco_t** mat_b_particionado = particionar_matriz(mat_b.matriz, mat_b.lin, mat_b.col, 1, NBLOCOS);
		matriz_bloco_t** mat_c_particionado = csubmatrizv2(mat_a.lin, mat_b.col, NBLOCOS);
		for(int n = 0; n < nro_tarefas; ++n) {
			args2[n].resultado = resultado;
			args2[n].mat_a = mat_a_particionado;
			args2[n].mat_b = mat_b_particionado;
			args2[n].mat_cPar = mat_c_particionado;
			pthread_create(&threads[n], NULL, multiplicarthbloco, (void*) (args2+n));
		}
		for(int n = 0; n < nro_tarefas; ++n) {
			pthread_join(threads[n], NULL);
		}
		for(int n = 0; n < nro_tarefas; ++n) {
			resultado = msomar(resultado, args2[n].resultado, 0);
		}
		end_time = wtime();
		printf("\tRuntime: %f\n", end_time - start_time);
		media_ThreadBlC += (end_time - start_time);
		sprintf(filename, "mult_thread_bloco%d.result", i);
		fmat = fopen(filename, "w");
		fileout_matriz(resultado, fmat);
		fclose(fmat);
	}
	media_ThreadBlC /= 2;
	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//    Comparar resultados e tempos
	printf("COMPARAR MATRIZ_SeqC c/ MATRIZ_SeqBlC\n");
	mcomparar(mat_mult[0], mat_mult[2]);
	printf("COMPARAR MATRIZ_SeqC c/ Matriz_ThreadC\n");
	mcomparar(mat_mult[0], mat_mult[4]);
	printf("COMPARAR MATRIZ SeqC c/ MATRIZ_ThreadBlC\n");
	mcomparar(mat_mult[0], resultado);
	printf("TEMPO MEDIO MATRIZ_SeqC: %f\n", media_seqC);
	printf("TEMPO MEDIO MATRIZ_SeqBlC: %f\n", media_seqBlC);
	printf("TEMPO MEDIO MATRIZ_ThreadC: %f\n", media_ThreadC);
	printf("TEMPO MEDIO MATRIZ_ThreadBlC: %f\n", media_ThreadBlC);


	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//                   Liberação de memória
	printf("\n\nLIBERANDO MAT_MULT\n\n");
	for (int ii=0; ii < 6; ii++) {
		printf("liberando %d\n", ii);
		mimprimir(mat_mult[ii]);
		mliberar(mat_mult[ii]);
		//printf("feito meu galo\t");
		free (mat_mult[ii]);
		//printf("feito mesmo\n");
	}
	printf("\n\nLIBERANDO MAT_BLOCO\n\n");
    for( int i = 0; i < NBLOCOS; ++i) {
        printf("liberando %d\n", i);
		mliberar(mat_BlA[i]->matriz);
        free(mat_BlA[i]->matriz);
        free(mat_BlA[i]->bloco);

        mliberar(mat_BlB[i]->matriz);
        free(mat_BlB[i]->matriz);
        free(mat_BlB[i]->bloco);

        mliberar(mat_BlC[i]->matriz);
        free(mat_BlC[i]->matriz);
        free(mat_BlC[i]->bloco);
    }

	mliberar(&mat_a);
	mliberar(&mat_b);
	free(mat_mult);
	// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
	pthread_mutex_destroy(&lock);
	return 0;
}
