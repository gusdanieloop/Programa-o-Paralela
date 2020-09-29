# GNU Makefile

CC = nvcc
#CC = gcc
CCFLAGS = #-Wall
LDFLAGS =
TARGET = 09_matMult
EXE =

all: $(TARGET)

%.o: %.cu
	$(CC) $(CCFLAGS) -c $< -lm

%: %.o
	$(CC) $(LDFLAGS) $^ -o $@



help:
	@echo
	@echo
	@echo "####### Exemplo de Execução GPU #######"
	@echo "./09_matMult"

clean:
	rm -f *.o *~ $(TARGET) *.map *.result
