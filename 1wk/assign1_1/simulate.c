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


typedef struct args_t {
    int i_start;
    int i_stop;

} args_t;

pthread_t *g_pthreads;

pthread_mutex_t wait_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  wait_cond = PTHREAD_COND_INITIALIZER;

double *old;
double *cur;
double *next;
double *temp;

int g_imax;
int g_num_threads;
int threads_busy;

/* Add any functions you may need (like a worker) here. */
void *calc_wave(void *s);
int init_threads(const int num_threads);
int clean_threads(const int num_threads);

/*
 * Function to be threaded
 */
void *calc_wave(void *s)
{
    args_t * args = (args_t*)s;

    for (int i = args->i_start; i < (args->i_stop); i++) {
        next[i] = (2*cur[i]) - old[i] + 0.15*(cur[i-1] - (2*cur[i] - cur[i+1]));
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

    return NULL;
}

/*
 * Initialize all threads and check if all threads have been made successfully.
 */
int init_threads(const int num_threads)
{
    int interval = g_imax / num_threads;

    /* Alloc enough space for the buffers */
    if (!(g_pthreads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads))) {
        fprintf(stderr, "Malloc of threads failed\n");
        return 2;
    }

    /* Initialize the threads */
    for (int i = 0; i < num_threads; i++) {

        /* Create the arguments structs. */
        args_t args;
        args.i_start = i * interval;
        args.i_stop = (i+1) * interval;


        if (i == num_threads) args.i_stop = g_imax;

        if (pthread_create(&g_pthreads[i], NULL, calc_wave, (void*)&args)) {
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
        if(pthread_join(g_pthreads[i], NULL)) {
            return 1;
        }
    }

    free(g_pthreads);
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

    /* Set all the global variables. */
    old = old_array;
    cur = current_array;
    next = next_array;

    g_imax = i_max;
    g_num_threads = num_threads;
    threads_busy = num_threads;

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
        printf("\n");
    }
}
