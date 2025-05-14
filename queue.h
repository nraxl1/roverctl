#ifndef QUEUE_H
#define QUEUE_H

/*
 * Internal library helping with queue stuff.
 * Implemented from scratch to cut down reliance on GNU code
 * This is not to be used as an API by the user under normal circumstances.
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct Node {
	void* data;
	size_t size;
struct Node* next;
} node_t;

typedef struct Queue {
	node_t* front;
	node_t* rear;
} queue_t;

void initialize_queue(queue_t* q);
int queue_is_empty(queue_t* q);
void enqueue(queue_t* q, void* data);
void* dequeue(queue_t* q);
void display_queue(queue_t* q, void (*print)(void*));
void print_int(void* data);
void print_string(void* data);

#endif /* QUEUE_H */
