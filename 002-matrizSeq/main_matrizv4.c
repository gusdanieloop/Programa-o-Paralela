#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "toolsv3.h"
#include "matrizv3.h"
#include "matriz-operacoesv3.h"

pthread_mutex_t lock;

int main(int argc, char* argv[]) {
    mymatriz mat_a, mat_b, mat_c;
    mymatriz** mult;
    matriz_bloco_t **mat_blA, **mat_blB, **mat_blC;
    param_t* args;
    param_bloco_t* argsbl;

    pthread_t* threads;
    char filename[100];
    FILE* fmat;
    int nr_line;
    int* vet_line = NULL;
    int N, M, La, Lb;
    int nro_tarefas;
    int nblocos = 4;

    double start_time, end_time, m_seq, m_bl, m_th, m_th_bl;

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
    mult = ( mymatriz**) malloc( sizeof( mymatriz*) * 8);
    for(int i = 0; i < 8; ++i ) {
        mult[i] = (mymatriz*) malloc ( sizeof( mymatriz));
    }

    //mat A
    fmat = fopen(argv[1],"r");
	if (fmat == NULL) {
		printf("Error: Na abertura dos arquivos.");
		exit(1);
	}
	extrai_parametros_matriz(fmat, &N, &La, &vet_line, &nr_line);
	mat_a.matriz = NULL;
	mat_a.lin = N;
	mat_a.col = La;
	if (malocar(&mat_a)) {
		printf ("ERROR: Out of memory\n");
	}
	filein_matriz (mat_a.matriz, N, La, fmat, vet_line, nr_line);
	free (vet_line);
	fclose(fmat);

    //mat B
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

    if (mat_a.lin < nro_tarefas || nro_tarefas == 0){
		nro_tarefas = mat_a.lin;
    }

    if (mat_a.lin < nblocos){
        nblocos = mat_a.lin;
    }
	threads = ( pthread_t*) malloc(sizeof(pthread_t) * nro_tarefas);
	args = ( param_t*) malloc( sizeof( param_t) * nro_tarefas);
	argsbl = ( param_bloco_t*) malloc( sizeof( param_bloco_t) * nro_tarefas);

    //mat C
    mat_c.matriz = NULL;
    mat_c.lin = mat_a.lin;
    mat_c.col = mat_b.col;
	if( malocar( &mat_c)) {
		printf("ERROR: Out of memory\n");
	}

    //MultiThreads
    for(int n = 0; n < nro_tarefas; ++n) {
		argsbl[n].tid = args[n].tid = n;
		argsbl[n].ntasks = args[n].ntasks = nro_tarefas;
		args[n].lini = n * (mat_a.lin / nro_tarefas);
		if( n == nro_tarefas - 1) 
			args[n].lfim = mat_a.lin;
		else
			args[n].lfim = args[n].lini + (mat_a.lin / nro_tarefas);
	    
        argsbl[n].blini = n * (nblocos / nro_tarefas);
		if( n == nro_tarefas - 1)
			argsbl[n].blfim = nblocos;
		else
			argsbl[n].blfim = argsbl[n].blini + (nblocos / nro_tarefas);
        
        args[n].mat_a = &mat_a;
		args[n].mat_b = &mat_b;
		args[n].mat_cPar = &mat_c;
	}

    // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//               Multiplicação Sequencial
    m_seq = 0.0;
    for(int i = 0; i < 2; ++i) {
        printf("\n ### Multiplicação %d: Sequencial ### \n", i);
        start_time = wtime();
        mult[i] = mmultiplicar(&mat_a, &mat_b, i);
        end_time = wtime();
        printf("\tRuntime: %f\n", end_time - start_time);
		m_seq += (end_time - start_time);
        sprintf(filename, "mult_seq%d.result", i);
        fmat = fopen(filename, "w");
        fileout_matriz(mult[i], fmat);
        fclose(fmat);
    }
	m_seq /= 2;
	// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

    // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//              Multiplicação Sequencial em Bloco
    m_bl = 0.0;
    for(int i = 2; i < 4; ++i) {
		mult[i]->matriz = NULL;
		mult[i]->lin = mat_a.lin;
		mult[i]->col = mat_b.col;
		if( malocar( mult[i])) {
			printf("ERROR: Out of memory\n");
		}
        printf("\n ### Multiplicação %d: Sequencial em Bloco  ### \n", i);
        start_time = wtime();
        mat_blA = particionar_matriz(mat_a.matriz, mat_a.lin, mat_a.col, 0, nblocos);
        mat_blB = particionar_matriz(mat_b.matriz, mat_b.lin, mat_b.col, 1, nblocos);
        mat_blC = csubmatrizv2(mat_a.lin, mat_b.col, nblocos);
        for(int j = 0; j < nblocos; ++j) {
            mmsubmatriz(mat_blA[j], mat_blB[j], mat_blC[j]);
			mult[i] = msomar(mult[i], mat_blC[j]->matriz, 0);
        }
        end_time = wtime();
        printf("\tRuntime: %f\n", end_time - start_time);
		m_bl += (end_time - start_time);
        sprintf(filename, "mult_seq_bloco%d.result", i);
        fmat = fopen(filename, "w");
        fileout_matriz(mult[i], fmat);
        fclose(fmat);
    }	
	m_bl /= 2;
	// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

    // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//              Multiplicação Multithread
	m_th = 0.0;
    for(int i = 4; i < 6; ++i) {
		mult[i]->matriz = NULL;
		mult[i]->lin = mat_a.lin;
		mult[i]->col = mat_b.col;
		if( malocar( mult[i])) {
			printf("ERROR: Out of memory\n");
		}
		printf("\n ### Multiplicação %d: Multithread ### \n", i);
		mzerar(&mat_c);
		start_time = wtime();
		for(int n = 0; n < nro_tarefas; ++n) {	
			pthread_create(&threads[n], NULL, multiplicarth, (void*) (args+n));
		}
		for(int n = 0; n < nro_tarefas; ++n) {
			pthread_join(threads[n], NULL);
		}
		end_time = wtime();
		mult[i] = msomar(mult[i], args[0].mat_cPar, 0);
		printf("\tRuntime: %f\n", end_time - start_time);
		m_th += (end_time - start_time);
		sprintf(filename, "mult_thread%d.result", i);
		fmat = fopen(filename, "w");
		fileout_matriz(mult[i], fmat);
		fclose(fmat);
	}
	m_th /= 2;
	//%%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%


    // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//              Multiplicação Multithread Bloco
    m_th_bl = 0.0;
	for(int i = 6; i < 8; ++i) {
		mult[i]->lin = mat_a.lin;
		mult[i]->col = mat_b.col;
		mult[i]->matriz = NULL;
		if (malocar(mult[i])) {
			printf ("ERROR: Out of memory\n");
		}
		printf("\n ### Multiplicação %d: Multithread Bloco ### \n", i);
		start_time = wtime();
		mat_blA = particionar_matriz(mat_a.matriz, mat_a.lin, mat_a.col, 0, nblocos);
		mat_blB = particionar_matriz(mat_b.matriz, mat_b.lin, mat_b.col, 1, nblocos);
		mat_blC = csubmatrizv2(mat_a.lin, mat_b.col, nblocos);
		for(int n = 0; n < nro_tarefas; ++n) {
			argsbl[n].resultado = mult[i];
			argsbl[n].mat_a = mat_blA;
			argsbl[n].mat_b = mat_blB;
			argsbl[n].mat_cPar = mat_blC;
			pthread_create(&threads[n], NULL, multiplicarthbloco, (void*) (argsbl+n));
		}
		for(int n = 0; n < nro_tarefas; ++n) {
			pthread_join(threads[n], NULL);
		}
		for(int n = 0; n < nro_tarefas; ++n) {
			mult[i] = msomar(mult[i], argsbl[n].resultado, 0);
		}
		end_time = wtime();
		printf("\tRuntime: %f\n", end_time - start_time);
		m_th_bl += (end_time - start_time);
		sprintf(filename, "mult_thread_bloco%d.result", i);
		fmat = fopen(filename, "w");
		fileout_matriz(mult[i], fmat);
		fclose(fmat);
	}
	m_th_bl /= 2;

    // %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//    Comparar resultados e tempos
	printf("COMPARAR MATRIZ_SeqC c/ MATRIZ_SeqBlC\n");
	mcomparar(mult[0], mult[2]);
	printf("COMPARAR MATRIZ_SeqC c/ Matriz_ThreadC\n");
	mcomparar(mult[0], mult[4]);
	printf("COMPARAR MATRIZ SeqC c/ MATRIZ_ThreadBlC\n");
	mcomparar(mult[0], mult[6]);
	printf("TEMPO MEDIO MATRIZ_SeqC: %f\n", m_seq);
	printf("TEMPO MEDIO MATRIZ_SeqBlC: %f\n", m_bl);
	printf("TEMPO MEDIO MATRIZ_ThreadC: %f\n", m_th);
	printf("TEMPO MEDIO MATRIZ_ThreadBlC: %f\n", m_th_bl);

    //liberar memoria
    printf("LIBERANDO MULT\n");
    for(int i = 0; i < 8; ++i) {
        mliberar(mult[i]);
        free(mult[i]);
    }
    printf("LIBERANDO BLOCOS\n");
    mliberar(mat_blA[0]->matriz);
    free(mat_blA[0]->matriz);
    free(mat_blA[0]->bloco);

    mliberar(mat_blB[0]->matriz);
    free(mat_blB[0]->matriz);
    free(mat_blB[0]->bloco);

    mliberar(mat_blC[0]->matriz);
    free(mat_blC[0]->matriz);
    free(mat_blC[0]->bloco);
    free(mat_blA);
    free(mat_blB);
    free(mat_blC);
    printf("LIBERANDO ARGS\n");
    free(args);
    printf("LIBERANDO ARGSBL\n");
    for(int i = 0; i < nro_tarefas; ++i) {
        mliberar(argsbl[i].resultado);
    }
    free(argsbl);
    return 0;
}