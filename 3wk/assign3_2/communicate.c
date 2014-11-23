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

    /* Get rank and number of nodes. */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num);

    /* Get the effective rank, this means that we consider the root of the message as rank 0, while
     * in fact it could be any rank.  */
    int effective_rank = ((rank - root) % num);


    /* If not root, wait for message. */
    if (effective_rank) {
        MPI_Recv(buffer, count, datatype, MPI_ANY_SOURCE, DTAG, communicator, &status);
    }

    /* Calculate the t at which a certain node enters the loop,
     * and calculate the max amount of iterations */
    t = ceil(log2(effective_rank));
    t_max = ceil(log2(num));


    /* Start spreading the message after it has been received. Root starts immediately. */
    for (int i = t; i < t_max; i++) {
        /* Calculate which node to send to in this iteration, also translate the effective
         * rank to actual rank. */
        int dest = (effective_rank + (int)pow(2, i));

        /* Only send the message if the destination is valid. */
        if (dest < num) {
            int actual_dest = ((dest + root) % num);
            fprintf(stderr,"Sending message from effectkve: %d, actual: %d\n\
                    to effective: %d and actual: %d.\n",effective_rank, rank,
                    ((dest - root) % num), dest);
            MPI_Send(buffer, count, datatype, dest, DTAG, communicator);
        }

    }
    return 1;
}

int main(int argc, char* argv[])
{

    int message = 4;
    MPI_Init(&argc, &argv);
    broadcast(&message, 1, MPI_INT, 4, MPI_COMM_WORLD);
    MPI_Finalize();

    return EXIT_SUCCESS;
}
