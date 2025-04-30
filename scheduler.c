#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// TODO: Remove this when not required for memcpy - see 'enqueue()'
#include <string.h>


// TODO: Wrap these in POSIX mutexes
typedef struct node_t {
	void* data;
	size_t size;
struct node_t* next;
} node_t;

typedef struct queue_t {
	node_t* front;
	node_t* rear;
} queue_t;

void handle_incoming_uart(){
	
}

void handle_outgoing_uart(){

}

/// Initializes a queue into the pointer of type 'queue_t*'
void initialize_queue(queue_t* q){
	q->front = q->rear = NULL;
}

int queue_is_empty(queue_t* q){
	return (q->front == NULL);
}

// TODO: Remove the malloc and memcpy shenenigans as they are not needed as long as the caller properly tracks the type of the variable q->data points to
/// Enqueues an element into the given queue
void enqueue(queue_t* q, void* data, size_t data_size) {
    node_t* newNode = (node_t*)malloc(sizeof(node_t));
    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    /* Queue owns the data
    newNode->data = malloc(data_size);  // Allocate memory for the data
    if (newNode->data == NULL) {
        printf("Memory allocation for data failed!\n");
        free(newNode);
        return;
    }
    memcpy(newNode->data, data, data_size);
    */

    // Queue holds a pointer to externally managed data
    newNode->data = data;
    newNode->next = NULL;
    
    if (queue_is_empty(q)) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

/// WARNING: CAN RETURN A NULL POINTER IF GIVEN AN EMPTY QUEUE
/// WARNING: THE RESULTING DATA IS TO BE DESTROYED ACCORDINGLY
/// Dequeues an element from the given queue
void* dequeue(queue_t* q) {
    if (queue_is_empty(q)) {
        printf("Queue is empty!\n");
        return NULL;
    }

    node_t* temp = q->front;
    void* data = temp->data;
    *size = temp->size;

    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    return data;
}

// --- Helper Functions for Display ---

/// Prints a queue with the given print function
void display_queue(queue_t* q, void (*print)(void*)) {
    if (queue_is_empty(q)) {
        printf("Queue is empty!\n");
        return;
    }

    node_t* current = q->front;
    while (current != NULL) {
        print(current->data);  // Print the data
        current = current->next;
    }
    printf("\n");
}

// --- For use in conjunction with display_queue ---

/// Print function for integers
void print_int(void* data) {
    printf("%d ", *(int*)data);
}

/// Print function for null-terminated strings
void print_string(void* data) {
    // TODO: Double check this
    printf("&s\n", data); 
}
