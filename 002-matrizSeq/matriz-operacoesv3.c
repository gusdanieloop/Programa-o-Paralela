#include <math.h>
#include <pthread.h>
#include "matriz-operacoesv3.h"

void print_submatriz(matriz_bloco_t* mat){
	int lin_comeco = mat->bloco->lin_inicio;
	int lin_fim = mat->bloco->lin_fim;
	int col_comeco = mat->bloco->col_inicio;
	int col_fim = mat->bloco->col_fim;
	for( int i = lin_comeco; i < lin_fim; ++i){
		for( int j = col_comeco; j < col_fim; ++j){
			printf("%d\t", mat->matriz->matriz[i][j]);
		}
		printf("\n");
	}
	return;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% MATRIZ %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
mymatriz* msomar (mymatriz* mat_a, mymatriz* mat_b, int tipo) {
	mymatriz* mat_c = NULL;

	if ((mat_a->lin != mat_b-> lin) || (mat_a->col != mat_b->col)){
		printf ("Erro: Matrizes incompatíveis!\n");
		exit(1);
	}

	mat_c = (mymatriz*) malloc (sizeof(mymatriz));
	mat_c->lin = mat_a->lin;
	mat_c->col = mat_a->col;

	if (malocar(mat_c)) {	printf ("ERROR: Out of memory\n"); }

	if (tipo == 1) {
		for (int i=0; i < mat_c->lin; i++)
		  for (int j=0; j < mat_c->col; j++)
					mat_c->matriz[i][j] = mat_a->matriz[i][j]+mat_b->matriz[i][j];
	} else {
		for (int j=0; j < mat_c->col; j++)
			for (int i=0; i < mat_c->lin; i++)
					mat_c->matriz[i][j] = mat_a->matriz[i][j]+mat_b->matriz[i][j];
	}
  return mat_c;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
mymatriz* mmultiplicar (mymatriz* mat_a, mymatriz* mat_b, int tipo) {
	mymatriz* mat_c = NULL;

	if (mat_a->col != mat_b->lin){
		printf ("Erro: Matrizes incompatíveis!\n");
		exit(1);
	}

	mat_c = (mymatriz*) malloc (sizeof(mymatriz));
	mat_c->lin = mat_a->lin;
	mat_c->col = mat_b->col;

	if (malocar(mat_c)) { printf ("Error: Out of memory\n"); }

	for (int i = 0; i < mat_c->lin; ++i)
		for (int j = 0; j < mat_c->col; ++j)
			mat_c->matriz[i][j] = 0;

	if (tipo == 1) {
		for (int i = 0; i < mat_c->lin; ++i)
			for (int j = 0; j < mat_c->col; ++j)
				for (int k = 0; k < mat_a->col; ++k)
					mat_c->matriz[i][j] += (mat_a->matriz[i][k] * mat_b->matriz[k][j]);
	}
	else {
		for (int j = 0; j < mat_c->col; ++j)
			for (int i = 0; i < mat_c->lin; ++i)
				for (int k = 0; k < mat_a->col; ++k)
					mat_c->matriz[i][j] += (mat_a->matriz[i][k] * mat_b->matriz[k][j]);
	}
	return mat_c;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int mmsubmatriz (matriz_bloco_t* mat_suba, matriz_bloco_t* mat_subb, matriz_bloco_t* mat_subc) {
	int a_lin_comeco = mat_suba->bloco->lin_inicio;
	int a_col_comeco = mat_suba->bloco->col_inicio;
	int b_col_comeco = mat_subb->bloco->col_inicio;
	int diff = mat_subb->bloco->lin_fim - mat_subb->bloco->lin_inicio;

	for( int i = 0, ia = a_lin_comeco; i < mat_subc->matriz->lin; ++i, ++ia) {
		for( int j = 0, jb = b_col_comeco; j < mat_subc->matriz->col; ++j, ++jb) {
			for( int k = a_col_comeco; k < a_col_comeco+diff; ++k){
				mat_subc->matriz->matriz[i][j] += mat_suba->matriz->matriz[ia][k] * mat_subb->matriz->matriz[k][jb];
			}
		}
	}
	//printf("MULTIPLICAÇÃO DE SUBMATRIZ COMPLETA\n");
	//print_submatriz(mat_subc);
	return 1;

}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
matriz_bloco_t** csubmatrizv2(int mat_lin, int mat_col, int divisor) {
	/*int** m = (int**) malloc( mat_lin * sizeof( int*));
	for(int i = 0; i < mat_lin; ++i) {
		m[i] = (int*) malloc( mat_col * sizeof(int));
	}*/

	matriz_bloco_t** submatriz = ( matriz_bloco_t**) malloc( divisor * sizeof( matriz_bloco_t*));
	if( !submatriz){
		printf("ERROR: Out of memory\n");
		return NULL;
	}
	for(int i = 0; i < divisor; ++i) {
		submatriz[i] = ( matriz_bloco_t*) malloc( sizeof( matriz_bloco_t));
		if(!submatriz[i]){
			printf("ERROR: Out of memory\n");
			return NULL;
		}
		mymatriz* matriz = (mymatriz*) malloc( sizeof(mymatriz));
		matriz->lin = mat_lin;
		matriz->col = mat_col;
		matriz->matriz = (int**) malloc( mat_lin * sizeof( int*));
		for(int i = 0; i < mat_lin; ++i) {
			matriz->matriz[i] = (int*) malloc( mat_col * sizeof(int));
		}
		submatriz[i]->matriz = matriz;
		submatriz[i]->bloco = (bloco_t*) malloc( sizeof( bloco_t));
		submatriz[i]->bloco->lin_fim = mat_lin;
		submatriz[i]->bloco->col_fim = mat_col;
		if(!submatriz[i]->bloco) {
			printf("ERROR: Out of memory\n");
			return NULL;
		}
	}
	return submatriz;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//divisor = qnt peças, orientacao = 0 -> vertical | 1 -> horizontal
matriz_bloco_t** particionar_matriz(int** matriz, int mat_lin, int mat_col, int orientacao, int divisor) {

	int lin_inicio = 0;
	int col_inicio = 0;
	int corte, resto;

	matriz_bloco_t** submatriz = csubmatrizv2(mat_lin, mat_col, divisor);
	if( orientacao) { //corte horizontal
		corte = mat_lin / divisor;
		resto = mat_lin % divisor;
	}
	else { //corte vertical
		corte = mat_col / divisor;
		resto = mat_col % divisor;
	}
	for( int i = 0; i < divisor; ++i) {
		submatriz[i]->bloco->lin_inicio = lin_inicio;
		submatriz[i]->bloco->col_inicio = col_inicio;
		if( orientacao){ //corte horizontal
			submatriz[i]->bloco->lin_fim = lin_inicio + corte;
			submatriz[i]->bloco->col_fim = mat_col;
			lin_inicio += corte;
			if( i == ( divisor - 1))
				submatriz[i]->bloco->lin_fim += resto; 
		}
		else {
			submatriz[i]->bloco->col_fim = col_inicio + corte;
			submatriz[i]->bloco->lin_fim = mat_lin;
			col_inicio += corte;
			if( i == ( divisor - 1))
				submatriz[i]->bloco->col_fim += resto;
		}
		submatriz[i]->matriz->matriz = matriz;
	}

	return submatriz;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Multiplicação Multithread

void* multiplicarth(void* arg) {
	param_t* p = ( param_t*)arg;
	//printf("THREAD %d: Linhas %d até %d\n", p->tid, p->lini, p->lfim);
	int i, j, k;
	for(i = p->lini; i < p->lfim; ++i) {
		for(j = 0; j < p->mat_b->col; ++j) {
			for(k = 0; k < p->mat_b->lin; ++k) {
				p->mat_cPar->matriz[i][j] += p->mat_a->matriz[i][k] * p->mat_b->matriz[k][j];
			}
		}
	}
	return NULL;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Multiplicação Multithread Bloco

void* multiplicarthbloco(void* arg) {
	param_bloco_t* p = ( param_bloco_t*)arg;
	//printf("THREAD %d: Blocos %d até %d\n", p->tid, p->blini, p->blfim);
	/*int alini, acini;
	int bcini, blini, blfim, diff;
	int clin, ccol;*/
	
	for( int b = p->blini; b < p->blfim; ++b) {
		//printf("THREAD: %d - BLOCO: %d\n", p->tid, b);
		mmsubmatriz(p->mat_a[b], p->mat_b[b], p->mat_cPar[b]);
		//print_submatriz(p->mat_cPar[b]);
		//pthread_mutex_lock(p->lock);
		//printf("RESULTADO ANTES --- THREAD: %d - BLOCO: %d\n", p->tid, b);
		//mimprimir(p->resultado);
		p->resultado = msomar(p->resultado, p->mat_cPar[b]->matriz, 0);
		//printf("RESULTADO DEPOIS --- THREAD: %d - BLOCO: %d\n", p->tid, b);
		//mimprimir(p->resultado);
		//pthread_mutex_unlock(p->lock);
		/*alini = p->mat_a[b]->bloco->lin_inicio;
		acini = p->mat_a[b]->bloco->col_inicio;
		bcini = p->mat_b[b]->bloco->col_inicio;
		blini = p->mat_b[b]->bloco->lin_inicio;
		blfim = p->mat_b[b]->bloco->lin_fim;
		diff = blfim - blini;

		clin = p->mat_cPar[b]->matriz->lin;
		ccol = p->mat_cPar[b]->matriz->col;

		for(int i = 0; i < clin; ++i, ++alini){
			for(int j = 0; j < ccol; ++j, ++bcini){
				for(int k = acini; k < acini+diff; ++k) {
					pthread_mutex_lock(p->lock);
					p->mat_cPar[b]->matriz->matriz[i][j] += p->mat_a[b]->matriz->matriz[i][k] * p->mat_b[b]->matriz->matriz[k][j];
					pthread_mutex_unlock(p->lock);
				}
			}
		}*/
	}
	return NULL;
}

/*
	int a_lin_comeco = mat_suba->bloco->lin_inicio;
	int a_col_comeco = mat_suba->bloco->col_inicio;
	int b_col_comeco = mat_subb->bloco->col_inicio;
	int diff = mat_subb->bloco->lin_fim - mat_subb->bloco->lin_inicio;

	for( int i = 0, ia = a_lin_comeco; i < mat_subc->matriz->lin; ++i, ++ia) {
		for( int j = 0, jb = b_col_comeco; j < mat_subc->matriz->col; ++j, ++jb) {
			for( int k = a_col_comeco; k < a_col_comeco+diff; ++k){
				mat_subc->matriz->matriz[i][j] += mat_suba->matriz->matriz[ia][k] * mat_subb->matriz->matriz[k][jb];
			}
		}
	}

*/