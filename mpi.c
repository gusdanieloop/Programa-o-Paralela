#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int rank, size;

    int buf;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(!rank)
        buf = 56;
    

    printf("Hello WOrld! I'm %d of %d and my buf is %d\n", rank, size, buf);

    MPI_Bcast(&buf, 1, MPI_INT, 0, MPI_COMM_WORLD);

    printf("I'm %d of %d and now my buf is %d\n", rank, size, buf);

    MPI_Finalize();
    return 0;
}