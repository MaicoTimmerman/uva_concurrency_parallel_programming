/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>

#include "simulate.h"


/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * num_threads: how many threads to use
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */


double *old, *cur, *next, *temp;
int g_t_max, g_i_max;

void calc_wave(void)
{
    for (int t = 0; t < g_t_max; t++) {
        #pragma omp parallel for schedule( static)
        for (int i = 0; i < g_i_max; i++) {
            next[i] = (2*cur[i]) - old[i] + (0.15*(cur[i-1] - (2*cur[i] - cur[i+1])));
        }
        temp = old;
        old = cur;
        cur = next;
        next = temp;
    }
    return;
}

double *simulate(const int i_max, const int t_max, const int num_threads,
        double *old_array, double *current_array, double *next_array)
{

    old = old_array;
    cur = current_array;
    next = next_array;
    g_i_max = i_max;
    g_t_max = t_max;

    return current_array;
}
