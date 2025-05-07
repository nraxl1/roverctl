#include "mutex_queue.h"
#include <libpynq.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-daemon.h>
#include <threads.h>
#include <uart.h>

#define UART_RX_PIN = IO_AR0
#define UART_TX_PIN = IO_AR1

struct timespec sleep_duration = {0, 50 * 1000000}; // 0.05 seconds

// TODO: Move all initialization to initialize.c
void initialize_networking() {
  switchbox_set_pin(UART_RX_PIN, SWB_UART0_RX);
  switchbox_set_pin(UART_TX_PIN, SWB_UART0_TX);
}

// Fix this mess later (not happening)
int outgoing_networking_handler(mutex_queue_t *outgoing_queue) {
  while (1) {
    if (!uart_has_space()) {
      thrd_sleep(&sleep_duration, NULL);
      sd_notify(0, "STATUS=Warning: UART outgoing queue congested (full)");

    } else if (mutex_queue_is_empty(outgoing_queue)) {
      thrd_sleep(&sleep_duration, NULL);
    } else {
      void *message = mutex_dequeue(outgoing_queue);
      uint size = strlen(message);
      if (size <= UINT8_MAX) {
        uart_send(UART0, size);
        for (const uint8_t *p = message; *p != '\0';
             p++) { // not sure if the deferencing works as I anticipate
        verify:
          if (!uart_has_space(UART0)) {
            thrd_sleep(&sleep_duration, NULL);
            sd_notify(0,
                      "STATUS=Warning: UART outgoing queue congested (full)");
            goto verify;
          }
          uart_send(UART0, *p);
        }
      } else {
        sd_notify(
            0,
            "STATUS=Warning: Outgoing message exceeds maximum length; ignored");
      }

      free(message); // Undefined behavior happens if uart_send() does not
                     // create copies of bytes being sent
    }
  }
  // if the send queue is full, sleep for 50ms
  // if you end up sleeping 10 times in a row, notify systemd (maybe)
}

int incoming_networking_handler(mutex_queue_t *incoming_queue) {
  // check for length
  // parse the rest accordingly

  while (1) {
    uint8_t expected_size = uart_recv(UART0);
    uint8_t *message =
        malloc((expected_size + 1) *
               sizeof(uint8_t)); // cast to uint8_t* because ISO C doesnt allow
                                 // indirection on void pointers
    while (expected_size > 0) {
      *message = uart_recv(UART0);
      message++;
      expected_size--;
    }
    *message = '\0';
    mutex_enqueue(incoming_queue, message);
    // ownership transferred to queue, no need to free()
  }
}
