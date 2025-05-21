#ifndef CONTROL_H
#define CONTROL_H

#include <pthread.h>
#include "mutex_queue.h"

int control(mutex_queue_t *outgoing, mutex_queue_t *incoming, pthread_mutex_lock *sensor_mutex, sensor_frame_t *sensor_frame);

#endif /* CONTROL_H */
