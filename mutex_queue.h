#ifndef MUTEX_QUEUE_H
#define MUTEX_QUEUE_H

#include <pthread.h>
#include "queue.h"

typedef struct MutexQueue {
    pthread_mutex_t lock;
    queue_t* shared_queue;
} mutex_queue_t;

void initialize_mutex_queue(mutex_queue_t* mq);

/// @return 1 if the queue is empty, 0 if not.
int mutex_queue_is_empty(mutex_queue_t* mq);

/// @warning Data being enqueued must be allocated on the heap using malloc.
void mutex_enqueue(mutex_queue_t* mq, void* data);

/// @warning This function will return a NULL pointer if an empty queue is passed into it.
/// @return  A pointer to an unknown type. The burden of tracking the type is on the caller.
void* mutex_dequeue(mutex_queue_t* mq);

#endif /* MUTEX_QUEUE_H */
