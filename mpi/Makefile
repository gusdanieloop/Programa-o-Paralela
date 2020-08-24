CC = mpicc
CCFLAGS = -Wall -O3
LDFLAGS = 
TARGET = main clean help

all: $(TARGET)

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

%: %.o
	$(CC) $(LDFLAGS) $^ -o $@

main: mainMPI.c matriz.o tools.o operacoes.o operacoes_mpi.o
	$(CC) $(CCFLAGS) matriz.o tools.o operacoes.o operacoes_mpi.o mainMPI.c -o $@ $(LDFLAGS)

clean:
	rm *.o *.map *.result

help:
	@echo
	@echo
	@echo "####### Exemplo de Execução #######"
	@echo "mpirun -np 4 -hostfile hostfile ./main 1000 1000 1000 1000 2"
	@echo "mpirun -np <número de processos> -hostfile <arquivo do hostfile> ./main <número de linhas para matriz A> <número de colunas para matriz A> <número de linhas para matriz B> <número de colunas para matriz B> <número de blocos>"
	@echo

