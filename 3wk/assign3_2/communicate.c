#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <openmpi/mpi.h>

#define DTAG 1

int broadcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm communicator);

int broadcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm communicator)
{
    int rank, num, t, t_max;
    MPI_Status status;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num);

    int effective_rank = ((rank - root) % num);


    if (effective_rank) {
        /* Is not root, wait for message. */
        MPI_Recv(buffer, count, datatype, MPI_ANY_SOURCE, DTAG, communicator, &status);
    }

    t = (effective_rank);
    t_max = ceil(log2(effective_rank));


    /* Start spreading the message after it has been received. Root starts immediately. */
    for (int i = t; t < t_max; t++) {
        int dest = (((effective_rank + (int)pow(2, t)) + root) % num);
        MPI_Send(buffer, count, datatype, dest, DTAG, communicator);
    }


    return 0;
}

int main(int argc, int* argv[])
{
    MPI_Init(&argc, &argv);


    return EXIT_SUCCESS;
}
