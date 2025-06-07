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

#ifdef TEST_BUILD
#include <string.h>
#endif /* TEST_BUILD */

// These GPIO pins can be changed with no issues because of the way switchbox works
#define UART_RX_PIN IO_AR0
#define UART_TX_PIN IO_AR1
#define UART_CHANNEL UART0

void *decoy_thread(void *arg)
{
  while (1)
  {
    // This thread does nothing, it is just a placeholder for testing purposes
    // Can also be configured to yield every 100ms to simulate a thread that does not do anything more accurately as 100ms is the standard quantum for this system
    // sleep(100);
    pthread_yield();
  }
  return NULL;
}

int main(int argc, char *argv[])
{

#ifdef TEST_BUILD
  int test_sensors = 0, test_control = 0, test_incoming = 0, test_outgoing = 0;
  for (int i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "all") == 0)
    {
      test_sensors = 1;
      test_control = 1;
      test_networking = 1;
    }
    if (strcmp(argv[i], "sensors") == 0)
      test_sensors = 1;
    if (strcmp(argv[i], "control") == 0)
      test_control = 1;
    if (strcmp(argv[i], "networking") == 0)
      test_networking = 1;
#endif /* TEST_BUILD */

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

#ifdef TEST_BUILD
    pthread_create(&incoming_networking_thread, NULL,
                   test_networking ? incoming_networking_handler : decoy_thread, &incoming_networking);
    pthread_create(&outgoing_networking_thread, NULL,
                   test_networking ? outgoing_networking_handler : decoy_thread, &outgoing_networking);
#else
  pthread_create(&incoming_networking_thread, NULL, incoming_networking_handler, &incoming_networking);
  pthread_create(&outgoing_networking_thread, NULL, outgoing_networking_handler, &outgoing_networking);
#endif /* TEST_BUILD */

    // -------------------------------------------------

    // Sensors
    // TODO: set switchbox, initialize global variables about sensors
    sensor_frame_t sensor_frame = {0};
    pthread_t sensor_thread;
    pthread_mutex_init(&sensor_frame.lock, NULL);
#ifdef TEST_BUILD
    pthread_create(&sensor_thread, NULL,
                   test_sensors ? handle_sensors : decoy_thread, &sensor_frame);
#else
  pthread_create(&sensor_thread, NULL, handle_sensors, &sensor_frame);
#endif /* TEST_BUILD */

    // -------------------------------------------------

    // Control
    control_args_t control_args = {
        .outgoing = &outgoing_networking,
        .incoming = &incoming_networking,
        .sensor_frame = &sensor_frame};
    pthread_t control_thread;
#ifdef TEST_BUILD
    pthread_create(&control_thread, NULL,
                   test_control ? control : decoy_thread, &control_args);
#else
  pthread_create(&control_thread, NULL, control, &control_args);
#endif

    // -------------------------------------------------
    // Wait for threads to finish (they won't, but this is just to keep the main thread alive)
    pthread_join(incoming_networking_thread, NULL);
    pthread_join(outgoing_networking_thread, NULL);
    pthread_join(sensor_thread, NULL);
    pthread_join(control_thread, NULL);
  }
