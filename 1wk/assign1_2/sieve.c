#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include "queue.h"

#define BUF_SIZE 8

int num_primes = 0;
int num_threads;

/* Argument thread for communication between filters */
typedef struct thread_args_t {
    pthread_t pthread;
    pthread_mutex_t buf_mutex;
    pthread_cond_t buf_cond;
    int thread_num;
    int buffer[BUF_SIZE];
    int buf_size;
} thread_args_t;

/*
 * Function to create a filter of the the first value given.
 */
void* filter(void *s) {
    /* int current; */
    /* queue_t* output_queue = NULL; */
    /*  */
    /* //TODO fix up this consumer with condition variables, so if queue empty, wait for alert. */
    /* while(1) { */
    /*     if (!queue_empty(input_queue)) { */
    /*         current = dequeue(input_queue); */
    /*         for (int i = filter_number; i <= current; i += i) { */
    /*             if (current == i) { */
    /*                 break; */
    /*             } */
    /*         } */
    /*         #<{(| Current passed filtering, pass on to next filter or create it if it isn't created yet. |)}># */
    /*         if (!output_queue) { */
    /*             output_queue = create_queue(); */
    /*  */
    /*             //TODO Start new thread doing this same function with the just */
    /*             // created output queue as input queue. */
    /*         } */
    /*         else { */
    /*             enqueue(current, output_queue); */
    /*         } */
    /*     } */
    /* } */
    return NULL;
}

/*
 * Main generator thread, starts up one filter and sends buffers with info.
 */
int main(int argc, char *argv[]) {

    int current_number;

    /* Parse commandline args: i_max t_max num_threads */
    if (argc < 2) {
        printf("Usage: %s num_primes\n", argv[0]);
        printf(" - num_primes: number of discrete primes calculated\n");
        printf("    * The num_primes is equal to the number of threads\n");
        printf("    * Should be >5 and <5000\n");

        return EXIT_FAILURE;
    }

    /* num_primes == max num_threads */
    num_threads = atoi(argv[1]);
    if (num_threads < 5 || num_threads > 5000 ) {
        return EXIT_FAILURE;
    }

    /* create the connection link between generator and the first filter */
    thread_args_t start_filter;

    /* Create the variables for mutual exclusion */
    pthread_mutex_init(&(start_filter.buf_mutex), NULL);
    pthread_cond_init(&(start_filter.buf_cond), NULL);

    start_filter.thread_num = 0;
    start_filter.buf_size = 0;

    /* Start the generator thread */
    if (pthread_create(
                &(start_filter.pthread),
                NULL,
                filter,
                (void*)&start_filter)) {
        fprintf(stderr, "An error happened while initializing generator");
        return EXIT_FAILURE;
    }

    /* Lock the buffer */
    pthread_mutex_lock(&(start_filter.buf_mutex));
    while (num_primes < num_threads) {


        /* Fill the buffer with 8 entries */
        for (int i = current_number; i < current_number + 8; i++) {
            start_filter.buffer[i % BUF_SIZE] = current_number;
        }

        /* Signal that the buffer is filled, and wait for the request of a
         * new buffer */
        pthread_cond_signal(&(start_filter.buf_cond));
        pthread_cond_wait(&(start_filter.buf_cond), &(start_filter.buf_mutex));
    }

    pthread_mutex_unlock(&(start_filter.buf_mutex));
    return EXIT_SUCCESS;
}
