arquivos onde são definidas as matrizes: matriz.h e matriz.c
arquivos onde são definidas as ferramentas: tools.h e tools.c
arquivos onde são feitas operações sobre as matrizes: operacoes.h e operacoes.c
arquivos onde são feitas operações sobre as matrizes usando OpenMPI: operacoes_mpi.h e operacoes_mpi.c

Uso:
	$ make clean
	$ make

Em seguida, executar o comando a seguir para executar o programa principal:
	$ mpirun -np <np> -hostfile <hostfile> ./mainMPI <nro_linhas_A> <nro_colunas_A> <nro_linhas_B> <nro_colunas_B> <n_blocos>

NOTA: Certifique-se que as matrizes A e B são compatíveis para as operações.

Sendo:
	<np>. número de processos 
	<hostfile>. arquivo do hostfile
    <nro_linhas_A> <nro_colunas_A>. respectivamente o número de linhas e colunas para a matriz A
    <nro_linhas_B> <nro_colunas_B>. respectivamente o número de linhas e colunas para a matriz B
    <n_blocos>. número de blocos para a multiplicação em bloco

### Exemplo ###
$ make clean;
$ make;
$ mpirun -np 4 -hostfile hostfile ./mainMPI 1000 1000 1000 1000 8