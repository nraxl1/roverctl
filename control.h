#ifndef CONTROL_H
#define CONTROL_H

#include <pthread.h>
#include "mutex_queue.h"

typedef struct ControlArgs
{
    mutex_queue_t *outgoing;
    mutex_queue_t *incoming;
    sensor_frame_t *sensor_frame;
} control_args_t;

void *control(void *args);

#endif /* CONTROL_H */
