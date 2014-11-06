/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "simulate.h"
#include "file.h"


struct args_t {
    int i_start;
    int i_stop;
    int thread_nr;
};

pthread_t *g_pthreads;

pthread_mutex_t wait_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  wait_cond = PTHREAD_COND_INITIALIZER;

double *old;
double *cur;
double *next;
double *temp;

int g_tmax;
int g_num_threads;
int threads_busy;

/* Add any functions you may need (like a worker) here. */
void *calc_wave(void *s);

/*
 * Function to be threaded
 */
void *calc_wave(void *s)
{
    struct args_t * args = (struct args_t*)s;

    fprintf(stderr, "istart: %d istop: %d thread_nr: %d\n", args->i_start, args->i_stop, args->thread_nr);
    for (int j = 0; j < g_tmax; j++) {
        for (int i = args->i_start; i < (args->i_stop); i++) {
            next[i] = (2*cur[i]) - old[i] + (0.15*(cur[i-1] - (2*cur[i] - cur[i+1])));
        }

        pthread_mutex_lock(&wait_lock);
        threads_busy--;
        if (threads_busy == 0) {
            temp = old;
            old = cur;
            cur = next;
            next = temp;
            threads_busy = g_num_threads;
            pthread_cond_broadcast(&wait_cond);
        } else {
            pthread_cond_wait(&wait_cond, &wait_lock);
        }
        pthread_mutex_unlock(&wait_lock);
    }

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

    /* Set all the global variables. */
    old = old_array;
    cur = current_array;
    next = next_array;
    g_tmax = t_max;
    g_num_threads = num_threads;
    threads_busy = num_threads;

    /* Thread creation */
    int interval = i_max / num_threads;

    /* Alloc enough space for the buffers */
    if (!(g_pthreads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads))) {
        fprintf(stderr, "Malloc of threads failed\n");
        exit(EXIT_FAILURE);
    }

    struct args_t args[num_threads];

    /* Initialize the threads */
    for (int i = 0; i < num_threads; i++) {

        /* Create the arguments structs. */
        args[i].i_start = i * interval;
        args[i].i_stop = (i+1) * interval;
        args[i].thread_nr = i;

        /* Leave the ends untouched. */
        if (i == 0) {
            args[i].i_start = 1;
        }
        if (i == num_threads - 1) {
            args[i].i_stop = i_max-1;
        }

        if (pthread_create(&g_pthreads[i], NULL, calc_wave, (void*)&(args[i]))) {
            fprintf(stderr, "An error happened while initializing threads.\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Join all the threads */
    for (int i = 0; i < num_threads; i++) {
        if(pthread_join(g_pthreads[i], NULL)) {
            fprintf(stderr, "An error happened while joining threads.\n");
            exit(EXIT_FAILURE);
        }
    }
    free(g_pthreads);

    return cur;
}
