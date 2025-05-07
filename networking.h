// networking.h
#ifndef NETWORKING_H
#define NETWORKING_H

#include "mutex_queue.h"

int outgoing_networking_handler(mutex_queue_t *outgoing_queue);
int incoming_networking_handler(mutex_queue_t *incoming_queue);

#endif /* NETWORKING_H */
