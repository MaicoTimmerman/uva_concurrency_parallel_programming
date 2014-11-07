#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include "queue.h"

#define BUF_SIZE 1

int g_num_threads;

/* Shared num_primes variable */
int g_num_primes;
pthread_mutex_t num_primes_mutex = PTHREAD_MUTEX_INITIALIZER;


/* Argument thread for communication between filters */
typedef struct thread_args_t {
    /* Thread info */
    pthread_t pthread;
    pthread_mutex_t buf_mutex;
    pthread_cond_t buf_cond;
    int filter_value;

    /* Buffer info */
    int buffer[BUF_SIZE];
    int buf_index; //equal to BUF_SIZE when full.
} thread_args_t;

/*
 * Function to create a filter of the the first value given.
 */
void* filter(void *s) {

    int filter_created = 0;
    int val;
    thread_args_t next_args;

    /* Get the arguments of this thread. */
    thread_args_t *args = (thread_args_t *)s;

    /* Print the new prime and raise the number of primes */
    pthread_mutex_lock(&num_primes_mutex);
    g_num_primes++;
    printf("\x1b[1A");
    printf("Current prime: (%d), Num of primes: (%d/%d)\n",
            args->filter_value,
            g_num_primes, g_num_threads);
    pthread_mutex_unlock(&num_primes_mutex);

    /* Create the connection link between this filter and the next. */
    pthread_mutex_init(&(next_args.buf_mutex), NULL);
    pthread_cond_init(&(next_args.buf_cond), NULL);
    next_args.buf_index = 0;

    /* Continue until enough primes have been generated */
    while (g_num_primes < g_num_threads) {

        pthread_mutex_lock(&(args->buf_mutex));

        /* If the input buffer is empty, wait for a
         * signal that it has been filled further. */
        while (!(args->buf_index > 0)) {
            pthread_cond_wait(
                    &(args->buf_cond),
                    &(args->buf_mutex));
        }

        /* Fetch the new value from the buffer */
        val = args->buffer[(args->buf_index) - 1];
        (args->buf_index)--;

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
            pthread_create(
                    &(next_args.pthread),
                    NULL,
                    filter,
                    (void*)&next_args);
            continue;
        }

        /* Input found and next filter exist, thus lock the output buffer */
        pthread_mutex_lock(&(next_args.buf_mutex));

        /* If the input buffer is empty, wait for a
         * signal that it has been filled further. */
        while (!(next_args.buf_index < BUF_SIZE)) {
            pthread_cond_wait(
                    &(next_args.buf_cond),
                    &(next_args.buf_mutex));
        }

        next_args.buffer[next_args.buf_index] = val;
        next_args.buf_index++;

        pthread_cond_signal(&(next_args.buf_cond));
        pthread_mutex_unlock(&(next_args.buf_mutex));

    }

    pthread_mutex_destroy(&(next_args.buf_mutex));
    pthread_cond_destroy(&(next_args.buf_cond));

    exit(EXIT_SUCCESS);
    return NULL;
}

/*
 * Main generator thread, starts up one filter and sends buffers with info.
 */
int main(int argc, char *argv[]) {

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

    /* create the connection link between generator and the first filter */
    thread_args_t start_filter;

    /* Create the variables for mutual exclusion */
    pthread_mutex_init(&(start_filter.buf_mutex), NULL);
    pthread_cond_init(&(start_filter.buf_cond), NULL);

    start_filter.filter_value = current_number;
    start_filter.buf_index = 0;

    printf("Num of primes: %d\n", g_num_primes);
    printf("Current prime: %d\n", 2);

    /* Start the first filter thread thread */
    if (pthread_create(
                &(start_filter.pthread),
                NULL,
                filter,
                (void*)&start_filter)) {
        fprintf(stderr, "An error happened while initializing generator\n");
        return EXIT_FAILURE;
    }

    while (g_num_primes < g_num_threads) {

        /* Lock the buffer */
        pthread_mutex_lock(&(start_filter.buf_mutex));

        /* If the buffer is full, wait for a
         * signal that items have been taken */
        while(!(start_filter.buf_index < BUF_SIZE)) {
            pthread_cond_wait(
                    &(start_filter.buf_cond),
                    &(start_filter.buf_mutex));
        }

        start_filter.buffer[start_filter.buf_index] = current_number;
        current_number+=2;
        start_filter.buf_index++;

        /* Signal that the buffer is filled, and wait for the request of a
         * new buffer */
        pthread_cond_signal(&(start_filter.buf_cond));
        pthread_mutex_unlock(&(start_filter.buf_mutex));
    }

    exit(EXIT_SUCCESS);
}
