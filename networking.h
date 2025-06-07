// networking.h
#ifndef NETWORKING_H
#define NETWORKING_H

#include "mutex_queue.h"

void *outgoing_networking_handler(void *outgoing_queue);
void *incoming_networking_handler(void *incoming_queue);

#endif /* NETWORKING_H */
