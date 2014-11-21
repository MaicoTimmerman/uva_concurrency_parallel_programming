/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>

#include "simulate.h"


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
        double *next, const int num_taks, const int task_id)
{

    /*
     * Your implementation should go here.
     */
    /*  send(left_neighbour, cur[1]) ; */
    MPI_Send(old[0], 1, MPI_DOUBLE, 0, MSG_WRITE, MPI_COMM_WORLD);
    MPI_Send(cur[0], 1, MPI_DOUBLE, 0, MSG_WRITE, MPI_COMM_WORLD);
    /* send(right_neighbour, cur[local_size]) ; */
    for (int i = 0; i < t_max; i++) {
        for (int i = 0; i < i_max; i++) {
            next[i] = (2*cur[i]) - old[i] + (0.15*(cur[i-1] - (2*cur[i] - cur[i+1])));
        }
        /* cur[local_size + 1] = receive(right_neighbour); */
        /* new[local_size ] = ...; */
        /* send(right_neighbour , new [local_size]); */
        /* cur[0] = receive(left_neighbour); */
        /* new[1] = ...; */
        /* send(left_neighbour , new [1]); */
        /* old = cur; */
        /* cur = new; */
    }
    /* discard = receive ( left_neighbour ) ; */
    /* discard = receive ( right_neighbour ) ; */

    /* You should return a pointer to the array with the final results. */
    return cur;
}
