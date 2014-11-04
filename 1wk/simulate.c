/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "simulate.h"


/* Add any global variables you may need. */
typedef struct args_struct_t {
    double *old_array;
    double *current_array;
    double *next_array;
    const int i_max;
} args_struct_t;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int current_index;


/* Add any functions you may need (like a worker) here. */
void *calc_wave(void *s)
{
    args_struct_t *args = (args_struct_t*)s;
    printf("In pthread:\n");
    printf("p1: %p\n", (void*)args->old_array);
    printf("p2: %p\n", (void*)args->current_array);
    printf("p3: %p\n", (void*)args->next_array);
    return NULL;
}

/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * num_threads: how many threads to use (excluding the main threads)
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */
double *simulate(const int i_max, const int t_max, const int num_threads,
        double *old_array, double *current_array, double *next_array)
{
    pthread_t p_threads[num_threads];

    /* Struct with arguments for the thread. */
    args_struct_t args;
    args.old_array = old_array;
    args.current_array = current_array;
    args.next_array = next_array;

    liveprint(current_array, i_max, 1);

    printf("In simulate:\n");
    printf("p1: %p\n", (void*)old_array);
    printf("p2: %p\n", (void*)current_array);
    printf("p3: %p\n", (void*)next_array);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&p_threads[i], NULL, calc_wave, (void*)&args);
    }

    /*
     * After each timestep, you should swap the buffers around. Watch out none
     * of the threads actually use the buffers at that time.
     */

    /* After Swapping print the current wave */
    liveprint(current_array, i_max, 0);

    for (int i = 0; i < num_threads; i++) {
        pthread_join(p_threads[i], NULL);
    }


    /* You should return a pointer to the array with the final results. */
    pthread_mutex_destroy(&lock);
    return current_array;
}

void liveprint(double *values, const int i_max, int clear)
{
    int lines_amount = 21; //has to be uneven!!
    int altitude = (lines_amount -1) / 2;


    /* Clear lines if not first time printing */
    if (clear) {
        for (int i = 0; i < lines_amount; i++) {
            printf("\e[1A");
        }
    }

    /* Print the Wave */
    for (int i = altitude; i >= (-1 * altitude); i--) {
        char filler_char = ' ';
        if (i == 0) {
            filler_char = '=';
        }

        /* Still needs to be percentualised, using the highest amplitude value */
        for (int j = 0; j < i_max; j++) {
            if (values[j] == i) {
                printf("+");
            }
            else {
                printf("%c", filler_char);
            }
        }
    }
}
