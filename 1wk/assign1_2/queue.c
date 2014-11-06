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

void destroy_queue(queue_t* queue) {
    queue_element_t* current = queue->queue_first;
    queue_element_t* new_current = NULL;
    while (current) {
        new_current = current->next;
        free(current);
        current = new_current;
    }
    free(queue);
    return;
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
        queue_element_t* new_item = malloc(sizeof(queue_element_t));
        new_item->previous = NULL;
        new_item->element = element;
        new_item->next = queue->queue_first;
        queue->queue_first->previous = new_item;
        queue->queue_first = new_item;
    }
}

/* Dequeue an element, cleaning up the removed element and returning the corresponding element. */
int dequeue(queue_t* queue) {
    /* Cannot dequeue when empty, will return -1 instead. */
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
    return -1;
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

int main(int argc, char *argv[]) {
    queue_t* queue1 = create_queue();
    queue_t* queue2 = create_queue();

    enqueue(1, queue1);
    enqueue(11, queue2);
    enqueue(2, queue1);
    enqueue(12, queue2);
    enqueue(13, queue2);
    printf("Queue2 = %d, queue 1 = %d\n", queue_size(queue2), queue_size(queue1));
    printf("%d\n", dequeue(queue1));
    printf("%d\n", queue_empty(queue1));
    printf("%d\n", dequeue(queue1));
    printf("%d\n", queue_empty(queue1));
    printf("end of queue 1\n");
    printf("%d\n", dequeue(queue2));
    printf("%d\n", dequeue(queue2));
    enqueue(21, queue2);
    printf("%d\n", dequeue(queue2));
    printf("%d\n", dequeue(queue2));
    printf("%d\n", dequeue(queue2));
    printf("%d\n", queue_empty(queue2));
    destroy_queue(queue2);
    enqueue(2, queue1);
    enqueue(3, queue1);
    destroy_queue(queue1);
    return 0;

}

