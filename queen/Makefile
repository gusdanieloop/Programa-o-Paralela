CC = g++
CCFLAGS = -Wall -fopenmp -O3
LDFLAGS = 
TARGET = main2 main3 clean help

all: $(TARGET)

%.o: %.cpp
	$(CC) $(CCFLAGS) -c $<

%: %.o
	$(CC) $(LDFLAGS) $^ -o $@

main: main.cpp
	$(CC) $(CCFLAGS) main.cpp -o $@ $(LDFLAGS)

main2: main2.cpp
	$(CC) $(CCFLAGS) main2.cpp -o $@ $(LDFLAGS)

main3: main3.cpp
	$(CC) $(CCFLAGS) main3.cpp -o $@ $(LDFLAGS)

clean:
	rm *.o

help:
	@echo
	@echo
	@echo "####### Exemplo de Execução #######"
	@echo "mpirun -np 2 ./main 10 4"
	@echo "mpirun -np <número de processos> ./main <tamanho_tabuleiro> <número de cavaleiros>"
	@echo

