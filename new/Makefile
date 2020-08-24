all: compile clear

compile:
	@gcc -Wall -c matriz.c
	@gcc -Wall -c matriz_op.c
	@gcc -Wall -O3 -fopenmp -pthread main.c matriz.o matriz_op.o

clear:
	@rm *.o *.result *.map

run:
	@./a.out
