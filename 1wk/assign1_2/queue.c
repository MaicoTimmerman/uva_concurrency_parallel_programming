#include <stdlib.h>
#include <stdio.h>

#include "queue.h"

/* Queue is implemented as a linked list, queue_element_t struct is one element in the list. */
struct queue_element_t {
    queue_element_t* previous;
    int element;
    queue_element_t* next;
};

/* Struct that hold the first and last items in the queue, used to differentiate between
 * different queues. */
struct queue_t {
    queue_element_t* queue_first;
    queue_element_t* queue_last;
};

/* Creates a new queue and give back a pointer used to manipulate that queue. */
queue_t* create_queue(void) {
    queue_t* new_queue = malloc(sizeof(queue_t));
    return new_queue;
}
/* Enqueue an element regardless of how many elements are currently in the queue. */
void enqueue(int element, queue_t* queue) {
    /* For enqueueing in an empty queue. */
    if (queue->queue_first == NULL) {
        queue->queue_first = malloc(sizeof(queue_element_t));
        queue->queue_first->previous = NULL;
        queue->queue_first->element = element;
        queue->queue_first->next = NULL;
        queue->queue_last = queue->queue_first;
    }
    /* For a non-empty queue. */
    else {
        queue_element_t* newitem = malloc(sizeof(queue_element_t));
        newitem->previous = NULL;
        newitem->element = element;
        newitem->next = queue->queue_first;
        queue->queue_first->previous = newitem;
        queue->queue_first = newitem;
    }
}

/* Dequeue an element, cleaning up the removed element and returning the corresponding element. */
int dequeue(queue_t* queue) {
    /* Cannot dequeue when empty, will return 0 instead. */
    if (!queue_empty(queue)) {
        int element = queue->queue_last->element;
        queue->queue_last->element = 0;
        /* For a queue of more than one element. */
        if (queue->queue_last->previous) {
            queue->queue_last = queue->queue_last->previous;
            free(queue->queue_last->next);
            queue->queue_last->next = NULL;
        }
        /* If the element happens to be the only element left. */
        else {
            free(queue->queue_last);
            queue->queue_last = NULL;
            queue->queue_first = NULL;
        }
        return element;
    }
    return 0;
}

/* Check if the queue is empty. Note that it returns true when it IS empty. */
int queue_empty(queue_t* queue) {
    if (queue->queue_last) {
        return 0;
    }
    return 1;
}

/* Count the amount of elements in the queue. */
int queue_size(queue_t* queue) {
    int count = 0;
    queue_element_t* current = queue->queue_first;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

