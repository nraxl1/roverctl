#ifndef MUTEX_QUEUE_H
#define MUTEX_QUEUE_H

#include <pthread.h>
#include "queue.h"

typedef struct Mutex {
    pthread_mutex_t lock;
    queue_t* shared_queue;
} mutex_queue_t;

void initialize_mutex_queue(mutex_queue_t* mq) {
        pthread_mutex_init(&mq->lock, NULL);
        initialize_queue(mq->shared_queue);
}

#endif
