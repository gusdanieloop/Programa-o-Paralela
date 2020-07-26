all: compile clear

compile:
	@gcc -Wall -c matriz.c
	@gcc -Wall -c matriz_op.c
	@gcc -Wall -O3 -pthread main.c matriz.o matriz_op.o

clear:
	@rm *.o

run:
	@./a.out
