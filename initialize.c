/*
* EVERYTHING RELATING TO SHARED GLOBAL STATE MUST BE INITIALIZED IN THIS FILE
* THIS INCLUDES ANY INITIALIZATION FUNCTION THAT MAPS MEMORY -> 'mmap()'
* 'malloc()' is allowed in individual threads
* any operation that requires interacting with the memory mappings used in a thread must be completed here
*/

#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdlib.h>
#include <libpynq.h>
#include <uart.h>
#include "mutex_queue.h"
#include_next "networking.h"

// These GPIO pins can be changed with no issues because of the way switchbox works
#define UART_RX_PIN = IO_AR0
#define UART_TX_PIN = IO_AR1
#define UART_CHANNEL = UART0

int main() {
  // --- initialize system-wide resources here ---
  pynq_init();
  switchbox_init();
  // ---------------------------------------------

  // --- initialize thread-specific resources here ---
  
  // Networking
  switchbox_set_pin(UART_RX_PIN, SWB_UART0_RX);
  switchbox_set_pin(UART_TX_PIN, SWB_UART0_TX);
  uart_init(UART_CHANNEL);
  mutex_queue_t incoming_networking;
  mutex_queue_t outgoing_networking;
  initialize_mutex_queue(&incoming_networking);
  initialize_mutex_queue(&outgoing_networking);
  pthread_t incoming_networking_thread;
  pthread_t outgoing_networking_thread;
  pthread_create(&incoming_networking_thread, NULL, incoming_networking_handler, void *restrict arg)
  
  // -------------------------------------------------


}
