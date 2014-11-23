/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include <openmpi/mpi.h>

#include "simulate.h"
/* #include "mpi.h" */


/* Add any global variables you may need. */


/* Add any functions you may need (like a worker) here. */


/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */
double *simulate(const int i_max, const int t_max, double *old, double *cur,
        double *next, const int num_tasks, const int task_id)
{

    MPI_Request req_left_old;
    MPI_Request req_right_old;
    MPI_Request req_left_cur;
    MPI_Request req_right_cur;

    /* send(left_neighbour, old[0]) */
    MPI_Isend(&(old[1]), 1, MPI_DOUBLE,
            ((((task_id -1) % num_tasks) + num_tasks) % num_tasks),
            0, MPI_COMM_WORLD, &req_left_old);

    /* send(right_neighbour, old[local_size]) */
    MPI_Isend(&(old[i_max - 1]), 1, MPI_DOUBLE,
            ((((task_id + 1) % num_tasks) + num_tasks) % num_tasks),
            0, MPI_COMM_WORLD, &req_right_old);

    /* recv(left_neighbour, old[0]) */
    MPI_Recv(&(old[0]), 1, MPI_DOUBLE,
            ((((task_id -1) % num_tasks) + num_tasks) % num_tasks),
            0, MPI_COMM_WORLD, NULL);

    /* recv(right_neighbour, old[local_size]) */
    MPI_Recv(&(old[i_max]), 1, MPI_DOUBLE,
            ((((task_id + 1) % num_tasks) + num_tasks) % num_tasks),
            0, MPI_COMM_WORLD, NULL);

    /* Wait for all the sync the initial layer */
    MPI_Barrier(MPI_COMM_WORLD);

    /* send(left_neighbour, cur[0]) */
    MPI_Isend(&(cur[1]), 1, MPI_DOUBLE,
            ((((task_id -1) % num_tasks) + num_tasks) % num_tasks),
            0, MPI_COMM_WORLD, &req_left_cur);
    /* send(right_neighbour, cur[local_size]) */
    MPI_Isend(&(cur[i_max - 1]), 1, MPI_DOUBLE,
            ((((task_id + 1) % num_tasks) + num_tasks) % num_tasks),
            0, MPI_COMM_WORLD, &req_right_cur);

    fprintf(stderr, "%d, Done initializing\n", task_id);

    int i = 0;
    for (; i < t_max; i++) {
        for (int j = 2; j < i_max - 2; j++) {
            next[j] = (2*cur[j]) - old[j] + (0.15*(cur[j-1] - (2*cur[j] - cur[j+1])));
        }
        MPI_Request req_left;
        MPI_Request req_right;

        fprintf(stderr, "%d, i: %d\n",task_id, i);

        /* cur[local_size] = receive(right_neighbour); */
        MPI_Recv(&(cur[i_max]), 1, MPI_DOUBLE,
                ((((task_id + 1) % num_tasks) + num_tasks) % num_tasks),
                i, MPI_COMM_WORLD, NULL);


        /* next[local_size-1] = ...; */
        next[i_max - 1] = (2*cur[i_max-1]) - old[i_max-1] +
            (0.15*(cur[i_max-1-1] - (2*cur[i_max-1] - cur[i_max-1+1])));

        /* send(right_neighbour , next[local_size-1]); */
        MPI_Isend(&(next[i_max - 1]), 1, MPI_DOUBLE,
                ((((task_id + 1) % num_tasks) + num_tasks) % num_tasks),
                i+1, MPI_COMM_WORLD, &req_right);


        /* cur[0] = receive(left_neighbour) */
        MPI_Recv(&(cur[0]), 1, MPI_DOUBLE,
                ((((task_id - 1) % num_tasks) + num_tasks) % num_tasks),
                i, MPI_COMM_WORLD, NULL);

        /* new[1] = ...; */
        next[1] = (2*cur[1]) - old[1] + (0.15*(cur[0] - (2*cur[1] - cur[2])));

        /* send(left_neighbour , new [1]); */
        MPI_Isend(&(next[i_max - 1]), 1, MPI_DOUBLE,
                ((((task_id - 1) % num_tasks) + num_tasks) % num_tasks),
                i+1, MPI_COMM_WORLD, &req_left);

        old = cur;
        cur = next;
        next = old;
        fprintf(stderr, "in i: %d\n", i);

    }
    fprintf(stderr, "na i: %d\n", i);

    double discard = 42.;
    /* discard last receive(left_neighbour); */
    MPI_Recv(&discard, 1, MPI_DOUBLE, (task_id + 1) % num_tasks,
                i, MPI_COMM_WORLD, NULL);
    /* discard last receive(right_neighbour);*/
    MPI_Recv(&discard, 1, MPI_DOUBLE, (task_id + 1) % num_tasks,
                i, MPI_COMM_WORLD, NULL);

    /* You should return a pointer to the array with the final results. */
    return cur;
}
