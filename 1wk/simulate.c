/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "simulate.h"


pthread_t *g_p_threads;
double *g_old_array;
double *g_current_array;
double *g_next_array;
const int *g_imax;
const int *g_num_threads;

/* Add any functions you may need (like a worker) here. */
void *calc_wave(void *s);
int init_threads(const int num_threads);
int clean_threads(const int num_threads);

/*
 * Function to be threaded
 */
void *calc_wave(void *s)
{
    int *thread_id = (int*)s;
    int interval = *g_imax / *g_num_threads;
    int start, stop;

    start = *thread_id * interval;
    stop = (*thread_id + 1) * interval;

    if (*thread_id == *g_num_threads) stop = *g_imax;

    for (int i = start; i < stop; i++) {
        g_next_array[i] = (2 * g_current_array[i]) - g_old_array[i] +
            0.15 * (g_current_array[i-1] -
                    (2 * g_current_array[i] - g_current_array[i+1]));
    }

    return NULL;
}

/*
 * Initialize all threads and check if all threads have been made successfully.
 */
int init_threads(const int num_threads)
{

    /* Alloc enough space for the buffers */
    if (!(g_p_threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads))) {
        fprintf(stderr, "Malloc of threads failed\n");
        return 2;
    }

    /* Initialize the threads */
    for (int i = 0; i < num_threads; i++) {
        int *arg = (int*)malloc(sizeof(*arg));
        *arg = i;
        if (pthread_create(&g_p_threads[i], NULL, calc_wave, (void*)arg)) {
            fprintf(stderr, "pthread_create failed\n");
            return 1;
        }
    }
    return 0;
}

/*
 * Clean all the threads used in the program.
 */
int clean_threads(const int num_threads)
{

    /* Join all the threads */
    for (int i = 0; i < num_threads; i++) {
        if(pthread_join(g_p_threads[i], NULL)) {
            return 1;
        }
    }

    free(g_p_threads);
    return 0;
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

    int t_current = 0;
    double *temp;

    /* Struct with arguments for the thread. */
    g_old_array = old_array;
    g_current_array = current_array;
    g_next_array = next_array;
    g_imax = &i_max;
    g_num_threads = &num_threads;

    /* for (int i = 0; i < i_max; i++) { */
    /*     printf("%f, ", next_array[i]); */
    /* } */
    /* printf("\n"); */


    /* Loop through all rows */
    while (t_current < t_max) {

        /* Thread creation */
        if (init_threads(num_threads)) {
            fprintf(stderr, "An error happened while initializing threads.\n");
            exit(EXIT_FAILURE);
        }

        /* Synchronize all threads by joining them. */
        if (clean_threads(num_threads)) {
            fprintf(stderr, "An error happened while cleaning threads.\n");
            exit(EXIT_FAILURE);
        }

        /* for (int i = 0; i < i_max; i++) { */
        /*     printf("%f, ", next_array[i]); */
        /* } */
        /* printf("\n"); */

        temp = g_old_array;
        g_old_array = g_current_array;
        g_current_array = g_next_array;
        g_next_array = temp;

        t_current++;
    }

    return current_array;
}

void liveprint(double *amplitudes, const int i_max, int clear)
{
    int lines_amount = 21; //has to be uneven!!
    int altitude = (lines_amount -1) / 2;


    /* Clear lines if not first time printing */
    if (clear) {
        for (int i = 0; i < lines_amount; i++) {
            printf("\x1b[1A");
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
            if (amplitudes[j] == i) {
                printf("+");
            }
            else {
                printf("%c", filler_char);
            }
        }
    }
}
