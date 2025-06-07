#include "mutex_queue.h"
#include <libpynq.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-daemon.h>
#include <threads.h>
#include <uart.h>

#define UART_RX_PIN IO_AR0
#define UART_TX_PIN IO_AR1

static const struct timespec sleep_duration = {0, 50 * 1000000}; // 0.05 seconds

#ifdef TEST_BUILD

// TODO: Move all initialization to initialize.c
void initialize_networking()
{
  switchbox_set_pin(UART_RX_PIN, SWB_UART0_RX);
  switchbox_set_pin(UART_TX_PIN, SWB_UART0_TX);
}

void yield_if_no_data()
{
  while (1)
  {
    if (uart_has_data(UART0) == 0)
    {
      pthread_yield();
    }
    else
    {
      return;
    }
  }
}
// Fix this mess later (not happening)
void *outgoing_networking_handler(void *outgoing_queue)
{
  mutex_queue_t *outgoing_queue = (mutex_queue_t *)outgoing_queue;
  while (1)
  {
    if (!uart_has_space())
    {
      // thrd_sleep(&sleep_duration, NULL);
      sd_notify(0, "STATUS=Warning: UART outgoing queue congested (full)");
      pthread_yield();
    }
    else if (mutex_queue_is_empty(outgoing_queue))
    {
      // thrd_sleep(&sleep_duration, NULL);
      pthread_yield();
    }
    else
    {
      void *message = mutex_dequeue(outgoing_queue);
      uint size = strlen((char *)message);
      if (size <= UINT8_MAX)
      {
        uart_send(UART0, size);
        for (const uint8_t *p = message; *p != '\0';
             p++)
        { // not sure if the deferencing works as I anticipate
        verify:
          if (!uart_has_space(UART0))
          {
            // thrd_sleep(&sleep_duration, NULL);
            sd_notify(0,
                      "STATUS=Warning: UART outgoing queue congested (full)");
            pthread_yield();
            goto verify;
          }
          uart_send(UART0, *p);
        }
      }
      else
      {
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

void *incoming_networking_handler(void *incoming_queue)
{
  mutex_queue_t *incoming_queue = (mutex_queue_t *)incoming_queue;
  // check for length
  // parse the rest accordingly

  while (1)
  {
    yield_if_no_data();
    uint8_t expected_size = uart_recv(UART0);
    uint8_t *message =
        malloc((expected_size + 1) *
               sizeof(uint8_t)); // cast to uint8_t* because ISO C doesnt allow
                                 // indirection on void pointers
    uint8_t *write_ptr = message;
    while (expected_size > 0)
    {
      yield_if_no_data();
      *write_ptr = uart_recv(UART0);
      *write_ptr++;
      expected_size--;
    }
    *write_ptr = '\0';
    mutex_enqueue(incoming_queue, message);
    // ownership transferred to queue, no need to free()
  }
}

#else

// TODO: Move all initialization to initialize.c
void initialize_networking()
{
  switchbox_set_pin(UART_RX_PIN, SWB_UART0_RX);
  switchbox_set_pin(UART_TX_PIN, SWB_UART0_TX);
}

void yield_if_no_data()
{
  while (1)
  {
    if (uart_has_data(UART0) == 0)
    {
      pthread_yield();
    }
    else
    {
      return;
    }
  }
}
// Fix this mess later (not happening)
void *outgoing_networking_handler(void *outgoing_queue)
{
  mutex_queue_t *outgoing_queue = (mutex_queue_t *)outgoing_queue;
  while (1)
  {
    if (!uart_has_space())
    {
      // thrd_sleep(&sleep_duration, NULL);
      sd_notify(0, "STATUS=Warning: UART outgoing queue congested (full)");
      pthread_yield();
    }
    else if (mutex_queue_is_empty(outgoing_queue))
    {
      // thrd_sleep(&sleep_duration, NULL);
      pthread_yield();
    }
    else
    {
      void *message = mutex_dequeue(outgoing_queue);
      uint size = strlen((char *)message);
      if (size <= UINT8_MAX)
      {
        uart_send(UART0, size);
        for (const uint8_t *p = message; *p != '\0';
             p++)
        { // not sure if the deferencing works as I anticipate
        verify:
          if (!uart_has_space(UART0))
          {
            // thrd_sleep(&sleep_duration, NULL);
            sd_notify(0,
                      "STATUS=Warning: UART outgoing queue congested (full)");
            pthread_yield();
            goto verify;
          }
          uart_send(UART0, *p);
        }
      }
      else
      {
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

void *incoming_networking_handler(void *incoming_queue)
{
  mutex_queue_t *incoming_queue = (mutex_queue_t *)incoming_queue;
  // check for length
  // parse the rest accordingly

  while (1)
  {
    yield_if_no_data();
    uint8_t expected_size = uart_recv(UART0);
    uint8_t *message =
        malloc((expected_size + 1) *
               sizeof(uint8_t)); // cast to uint8_t* because ISO C doesnt allow
                                 // indirection on void pointers
    uint8_t *write_ptr = message;
    while (expected_size > 0)
    {
      yield_if_no_data();
      *write_ptr = uart_recv(UART0);
      *write_ptr++;
      expected_size--;
    }
    *write_ptr = '\0';
    mutex_enqueue(incoming_queue, message);
    // ownership transferred to queue, no need to free()
  }
}

#endif /* TEST_BUILD */
