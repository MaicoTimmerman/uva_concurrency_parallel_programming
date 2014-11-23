#include <stdio.h>
#include <stdlib.h>
#include <openmpi/mpi.h>

int broadcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm communicator);

int broadcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm communicator)
{
    return 0;
}

int main(int argc, int* argv[])
{
    return EXIT_SUCCESS;
}
