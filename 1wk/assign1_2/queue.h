/* Contains queue functions. */

#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue_element_t queue_element_t;
typedef struct queue_t queue_t;

void enqueue(int element, queue_t* queue);
int dequeue(queue_t* queue);
int queue_empty(queue_t* queue);
int queue_size(queue_t* queue);

#endif
