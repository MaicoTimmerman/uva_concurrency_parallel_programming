#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include "queue.h"

#define BUF_SIZE 8

int natural = 0;

void filter(int filter_number, queue_t* input_queue) {
    int current;
    queue_t* output_queue = NULL;

    //TODO fix up this consumer with condition variables, so if queue empty, wait for alert.
    while(1) {
        if (!queue_empty(input_queue)) {
            current = dequeue(input_queue);
            for (int i = filter_number; i <= current; i += i) {
                if (current == i) {
                    break;
                }
            }
            /* Current passed filtering, pass on to next filter or create it if it isn't created yet. */
            if (!output_queue) {
                output_queue = create_queue();

                //TODO Start new thread doing this same function with the just
                // created output queue as input queue.
            }
            else {
                enqueue(current, output_queue);
            }
        }
    }
}

// Voorbeeld
typedef struct thread_args {
    pthread_mutex_t buf_mutex;
    pthread_cond_t buf_cond;
    int buffer[BUF_SIZE];
} thread_args;
