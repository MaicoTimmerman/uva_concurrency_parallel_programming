#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include "timer.h"
#include "queue.h"

#define BUF_SIZE 1

int g_num_threads;

/* Shared num_primes variable */
int g_num_primes;
pthread_mutex_t num_primes_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Shared thread vector */
int g_pthread_counter;
pthread_t *g_pthreads;
pthread_mutex_t g_pthread_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct buffer_t {
    int buf_size;
    int write_index;
    int read_index;
    int values[BUF_SIZE];
} buffer_t;

/* Argument thread for communication between filters */
typedef struct thread_args_t {
    /* Thread info */
    pthread_mutex_t buf_mutex;
    pthread_cond_t buf_cond;
    int filter_value;

    /* Buffer info */
    buffer_t buffer;
} thread_args_t;

/*
 * Function to create a filter of the the first value given.
 */
void* filter(void *s) {

    int val;
    double time;
    thread_args_t next_args;
    int filter_created = 0;

    /* Get the arguments of this thread. */
    thread_args_t *args = (thread_args_t *)s;

    /* Print the new prime and raise the number of primes */
    pthread_mutex_lock(&num_primes_mutex);
    g_num_primes++;\
    printf("Num of primes: %d\n", g_num_primes);
    printf("Current prime: %d\n", args->filter_value);
    pthread_mutex_unlock(&num_primes_mutex);

    /* Create the connection link between this filter and the next. */
    pthread_mutex_init(&(next_args.buf_mutex), NULL);
    pthread_cond_init(&(next_args.buf_cond), NULL);
    next_args.buffer.read_index = 0;
    next_args.buffer.write_index = 0;
    next_args.buffer.buf_size = 0;

    /* Continue until enough primes have been generated */
    while (g_num_primes < g_num_threads) {

        pthread_mutex_lock(&(args->buf_mutex));

        /* If the input buffer is empty, wait for a
         * signal that it has been filled further. */
        while (!(args->buffer.buf_size > 0)) {
            pthread_cond_wait(
                    &(args->buf_cond),
                    &(args->buf_mutex));
        }

        /* Fetch the new value from the buffer */
        val = args->buffer.values[(args->buffer.buf_size) - 1];
        args->buffer.read_index++;

        /* Release the input buffer */
        pthread_cond_signal(&(args->buf_cond));
        pthread_mutex_unlock(&(args->buf_mutex));


        /* If the val is dividable by this filter number, it is not
         * prime, therefor discard and try again. */
        if ((val % (args->filter_value)) == 0) {
            continue;
        }


        /* If it is not dividable, then pass to the next filter.
         * If no such filter exist, then a prime has been found. */
        if (!filter_created) {
            filter_created = 1;
            next_args.filter_value = val;

            /* Create thread and raise counter */
            pthread_mutex_lock(&g_pthread_mutex);
            pthread_create(
                    &(g_pthreads[g_pthread_counter]),
                    NULL,
                    filter,
                    (void*)&next_args);
            g_pthread_counter++;
            pthread_mutex_unlock(&g_pthread_mutex);
            continue;
        }

        /* Input found and next filter exist, thus lock the output buffer */
        pthread_mutex_lock(&(next_args.buf_mutex));

        /* If the input buffer is empty, wait for a
         * signal that it has been filled further. */
        while (!(next_args.buffer.buf_size == 0)) {
            pthread_cond_wait(
                    &(next_args.buf_cond),
                    &(next_args.buf_mutex));
        }

        next_args.buffer.values[next_args.buffer.write_index] = val;
        next_args.buffer.write_index++;

        pthread_cond_signal(&(next_args.buf_cond));
        pthread_mutex_unlock(&(next_args.buf_mutex));

    }

    pthread_mutex_destroy(&(next_args.buf_mutex));
    pthread_cond_destroy(&(next_args.buf_cond));

    /* time = timer_end(); */
    /* printf("%g\n", time); */

    return NULL;
}

/*
 * Main generator thread, starts up one filter and sends buffers with info.
 */
int main(int argc, char *argv[]) {

    double time;
    int current_number = 3;
    g_num_primes = 1;

    /* Parse commandline args: i_max t_max num_threads */
    if (argc < 2) {
        printf("Usage: %s num_primes\n", argv[0]);
        printf(" - num_primes: number of discrete primes calculated\n");
        printf("    * The num_primes is equal to the number of threads\n");
        printf("    * Should be >5 and <5000\n");

        return EXIT_FAILURE;
    }

    /* num_primes == max num_threads */
    g_num_threads = atoi(argv[1]);
    if (g_num_threads < 5 || g_num_threads > 5000 ) {
        return EXIT_FAILURE;
    }

    /* The max number of threads is equal to the number of primes */
    g_pthread_counter = 0;
    if (!(g_pthreads = (pthread_t *)malloc(sizeof(pthread_t) * g_num_threads))) {
        fprintf(stderr, "Malloc of threads failed\n");
        exit(EXIT_FAILURE);
    }

    /* create the connection link between generator and the first filter */
    thread_args_t start_filter;

    /* Create the variables for mutual exclusion */
    pthread_mutex_init(&(start_filter.buf_mutex), NULL);
    pthread_cond_init(&(start_filter.buf_cond), NULL);

    start_filter.filter_value = current_number;
    start_filter.buffer.read_index = 0;
    start_filter.buffer.write_index = 0;

    timer_start();

    /* Start the first filter thread thread */
    pthread_mutex_lock(&g_pthread_mutex);
    if (pthread_create(
                &(g_pthreads[g_pthread_counter]),
                NULL,
                filter,
                (void*)&start_filter)) {
        fprintf(stderr, "An error happened while initializing generator\n");
        return EXIT_FAILURE;
    }
    g_pthread_counter++;
    pthread_mutex_unlock(&g_pthread_mutex);

    while (g_num_primes < g_num_threads) {

        /* Lock the buffer */
        pthread_mutex_lock(&(start_filter.buf_mutex));

        /* If the buffer is full, wait for a
         * signal that items have been taken */
        while(!(start_filter.buffer.buf_size == BUF_SIZE)) {
            pthread_cond_wait(
                    &(start_filter.buf_cond),
                    &(start_filter.buf_mutex));
        }

        start_filter.buffer.values[start_filter.buffer.write_index] = current_number;
        current_number+=2;
        start_filter.buffer.write_index++;

        /* Signal that the buffer is filled, and wait for the request of a
         * new buffer */
        pthread_cond_signal(&(start_filter.buf_cond));
        pthread_mutex_unlock(&(start_filter.buf_mutex));
    }

    time = timer_end();
    printf("%g\n", time);

    exit(EXIT_SUCCESS);
}
