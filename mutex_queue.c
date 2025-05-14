#include <pthread.h>
#include "queue.h"

typedef struct MutexQueue {
    pthread_mutex_t lock;
    queue_t* shared_queue;
} mutex_queue_t;

void initialize_mutex_queue(mutex_queue_t* mq) {
        pthread_mutex_init(&mq->lock, NULL);
        initialize_queue(mq->shared_queue);
}

/// @return 1 if the queue is empty, 0 if not.
int mutex_queue_is_empty(mutex_queue_t* mq){
    pthread_mutex_lock(&mq->lock);
    int return_value = queue_is_empty(mq->shared_queue);
    pthread_mutex_unlock(&mq->lock);
    return return_value;
}

/// @warning Data being enqueued must be allocated on the heap using malloc.
void mutex_enqueue(mutex_queue_t* mq, void* data){
    pthread_mutex_lock(&mq->lock);
    enqueue(mq->shared_queue, data);
    pthread_mutex_unlock(&mq->lock);
}

/// @warning This function will return a NULL pointer if an empty queue is passed into it.
/// @return  A pointer to an unknown type. The burden of tracking the type is on the caller.
void* mutex_dequeue(mutex_queue_t* mq){
    pthread_mutex_lock(&mq->lock);
    void* return_value = dequeue(mq->shared_queue);
    pthread_mutex_unlock(&mq->lock);
    return return_value;
}
 

