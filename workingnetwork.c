// very sloppy, last second fixes and adding everything to 1 file
#include <stdint.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/select.h>
#include <unistd.h>
#include <libpynq.h>
#include <switchbox.h>
#include <pinmap.h>

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h> // For isatty, read
#include <sys/select.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <openssl/evp.h> // Use EVP instead of MD5'
#include <pthread.h>
#include <stdlib.h>
#include <sys/queue.h>

#define BUFFER_SIZE 255 // Hard cap due to bridge protocol requirements

struct queue_entry_t {
    char *data;
    SIMPLEQ_ENTRY(queue_entry_t) entries;
};
typedef struct queue_entry_t queue_entry_t;

SIMPLEQ_HEAD(queue_head, queue_entry_t);

typedef struct {
    struct queue_head head;
    pthread_mutex_t lock;
} queue_t;


void json_add_string(char *buffer, size_t *pos, const char *key, const char *value) {
    if (*pos > 1) buffer[(*pos)++] = ',';
    *pos += snprintf(buffer + *pos, BUFFER_SIZE - *pos, "\"%s\":\"%s\"", key, value);
}

void json_add_int(char *buffer, size_t *pos, const char *key, int value) {
    if (*pos > 1) buffer[(*pos)++] = ',';
    *pos += snprintf(buffer + *pos, BUFFER_SIZE - *pos, "\"%s\":%d", key, value);
}

void json_add_double(char *buffer, size_t *pos, const char *key, double value) {
    if (*pos > 1) buffer[(*pos)++] = ',';
    *pos += snprintf(buffer + *pos, BUFFER_SIZE - *pos, "\"%s\":%g", key, value);
}

void json_serialize_raw(char *buffer, const char *format, va_list args) {
    size_t pos = 0;
    buffer[pos++] = '{';
    const char *fmt = format;
    int arg_count = strlen(format);
    for (int i = 0; i < arg_count; ++i) {
        const char *key = va_arg(args, const char *);
        switch (fmt[i]) {
            case 's': json_add_string(buffer, &pos, key, va_arg(args, const char *)); break;
            case 'i': json_add_int(buffer, &pos, key, va_arg(args, int)); break;
            case 'd':
            case 'f': json_add_double(buffer, &pos, key, va_arg(args, double)); break;
            default: fprintf(stderr, "Invalid type string provided.\n"); break;
        }
    }
    buffer[pos++] = '}';
    buffer[pos] = '\0';
}

// Compute MD5 hash as a hex string using EVP_Q_digest
void simple_hash(const void *data, size_t len, char *out_hex) {
    unsigned char digest[EVP_MAX_MD_SIZE];
    size_t digest_len;
    if (EVP_Q_digest(NULL, "MD5", NULL, data, len, digest, &digest_len)) {
        // Output only the first 32 bits (4 bytes, 8 hex chars)
        for (int i = 0; i < 4; ++i) {
            sprintf(out_hex + i * 2, "%02x", digest[i]);
        }
        out_hex[8] = '\0';
    } else {
        out_hex[0] = '\0'; // Error, set empty string
    }
}

// Wrapper function with MD5 checksum and timestamp
void json_serialize(char *buffer, const char *format, ...) {
    va_list args;

    // Serialize the payload into a temp buffer to calculate checksum
    char payload_buf[BUFFER_SIZE];
    va_start(args, format);
    json_serialize_raw(payload_buf, format, args);
    va_end(args);
    size_t payload_len = strlen(payload_buf);

    // Start the outer object
    size_t pos = 0;
    pos += snprintf(buffer + pos, BUFFER_SIZE - pos, "{");

    // Add payload to the output
    pos += snprintf(buffer + pos, BUFFER_SIZE - pos, "\"pl\":%s,", payload_buf);

    // Add MD5 checksum (as hex)
    char md5_hex[9]; // 8 hex digits + null terminator
    simple_hash(payload_buf, payload_len, md5_hex);
    pos += snprintf(buffer + pos, BUFFER_SIZE - pos, "\"ck\":\"%s\",", md5_hex);

    // Add timestamp
    time_t now = time(NULL);
    pos += snprintf(buffer + pos, BUFFER_SIZE - pos, "\"ts\":%ld}", (long)now);
}

    // Create a new, empty queue.
queue_t *queue_create() {
    queue_t *q = malloc(sizeof(queue_t));
    if (!q) return NULL;
    SIMPLEQ_INIT(&q->head);
    pthread_mutex_init(&q->lock, NULL);
    return q;
}

// Check if the queue is empty.
int queue_is_empty(queue_t *q) {
    int empty;
    pthread_mutex_lock(&q->lock);
    empty = SIMPLEQ_EMPTY(&q->head);
    pthread_mutex_unlock(&q->lock);
    return empty;
}

// Enqueue a string. The queue takes ownership of the string.
int queue_enqueue(queue_t *q, char *str) {
    queue_entry_t *new_entry = malloc(sizeof(queue_entry_t));
    if (!new_entry) return -1;
    new_entry->data = str;
    pthread_mutex_lock(&q->lock);
    SIMPLEQ_INSERT_TAIL(&q->head, new_entry, entries);
    pthread_mutex_unlock(&q->lock);
    return 0;
}

// Dequeue a string. Caller must free the returned string.
char *queue_dequeue(queue_t *q) {
    pthread_mutex_lock(&q->lock);
    queue_entry_t *entry = SIMPLEQ_FIRST(&q->head);
    if (!entry) {
        pthread_mutex_unlock(&q->lock);
        return NULL;
    }
    SIMPLEQ_REMOVE_HEAD(&q->head, entries);
    pthread_mutex_unlock(&q->lock);
    char *str = entry->data;
    free(entry);
    return str;
}

// Destroy the queue and all strings it contains.
void queue_destroy(queue_t *q) {
    char *str;
    while ((str = queue_dequeue(q)) != NULL) {
        free(str);
    }
    pthread_mutex_destroy(&q->lock);
    free(q);
}




// Mock handles and pointers (not used, but kept for compatibility)
// static void* uart_handles[2]; // Adjust NUM_UARTS as needed
// static void* uart_ptrs[2] = { NULL, NULL };

// void uart_init(const int uart) {
//     // No-op for terminal mock
//     (void)uart;
// }

// void uart_destroy(const int uart) {
//     // No-op for terminal mock
//     (void)uart;
// }

// void uart_send(const int uart, const uint8_t data) {
//     (void)uart; // Ignore UART number for terminal
//     putchar(data);
//     fflush(stdout); // Ensure output is visible immediately
// }

// uint8_t uart_recv(const int uart) {
//     (void)uart; // Ignore UART number for terminal
//     int c = getchar();
//     if (c == EOF) return 0; // Handle EOF (shouldn't happen in normal use)
//     return (uint8_t)c;
// }


// bool uart_has_data(const int uart) {
//     (void)uart; // Unused parameter

//     fd_set set;
//     struct timeval timeout = {0, 0};
//     FD_ZERO(&set);
//     FD_SET(STDIN_FILENO, &set);

//     // Check if there is data ready to read
//     if (select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout) <= 0) {
//         return false; // No data ready
//     }

//     // There is data, so try to read a byte
//     int c = getchar();
//     if (c == EOF) {
//         // EOF reached, return false
//         return false;
//     } else {
//         // Not EOF, put the byte back and return true
//         ungetc(c, stdin);
//         return true;
//     }
// }


// bool uart_has_space(const int uart) {
//     (void)uart;
//     // Terminal always has space to send data (unless output is blocked)
//     return true;
// }

// void uart_reset_fifos(const int uart) {
//     (void)uart;
//     // No-op for terminal mock
// }


void yield_if_no_data()
{
  while(1) {
  if (uart_has_data(UART0) == 0)
  {
    // thrd_sleep(&sleep_duration, NULL);
    sched_yield();
  } 
  else 
    {
      return; // data is available, exit the loop
    }
  }
}

// Fix this mess later (not happening)
void *outgoing_networking_handler(void *outgoing_queue_v)
{
  queue_t *outgoing_queue = (queue_t *)outgoing_queue_v;

  while (1)
  {
    if (!uart_has_space(UART0))
    {
      // thrd_sleep(&sleep_duration, NULL);
      sched_yield();
    }
    else if (queue_is_empty(outgoing_queue))
    {
      // thrd_sleep(&sleep_duration, NULL);
      sched_yield();
    }
    else
    {
      void *message = queue_dequeue(outgoing_queue);
      size_t size = strlen((char *)message);
      if (size <= UINT32_MAX)
      {
        uint8_t bytes[4];

    // Copy bytes from value to array (note: endianness matters!)
    for (int i = 0; i < 4; i++) {
        bytes[i] = (size >> (i * 8)) & 0xFF;
    }

    // Print bytes
    for (int i = 0; i < 4; i++) {
        uart_send(UART0, bytes[i]);
    }

        for (const uint8_t *p = message; *p != '\0';
             p++)
        { // not sure if the deferencing works as I anticipate
        verify:
          if (!uart_has_space(UART0))
          {
            // thrd_sleep(&sleep_duration, NULL);
            
            sched_yield();
            goto verify;
          }
          uart_send(UART0, *p);
        }
      }

      free(message); // Undefined behavior happens if uart_send() does not
                     // create copies of bytes being sent
                     // source code says its passed by value, so it should be
                     // safe to free here
    }
  }
}

uint32_t assemble_uint32(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3) {
    return ((uint32_t)byte3 << 24) | ((uint32_t)byte2 << 16) |
           ((uint32_t)byte1 << 8)  | ((uint32_t)byte0);
}


void *incoming_networking_handler(void *incoming_queue_v)
{
  queue_t *incoming_queue = (queue_t *)incoming_queue_v;
  // check for length
  // parse the rest accordingly

  while (1)
  {
    check_data:
    yield_if_no_data();
    uint8_t expected_size1 = uart_recv(UART0);
    uint8_t expected_size2 = uart_recv(UART0);
    uint8_t expected_size3 = uart_recv(UART0);
    uint8_t expected_size4 = uart_recv(UART0);
    uint32_t expected_size = assemble_uint32(expected_size1, expected_size2, expected_size3, expected_size4);
    if (expected_size == 0)
    {
      goto check_data; // invalid size, attempting to flush 0 bytes
                        // this might be a standard sequence after initialization
    }
    uint8_t *message =
        malloc((expected_size + 1) *
               sizeof(uint8_t)); // cast to uint8_t* because ISO C doesnt allow
                                 // indirection on void pointers
    uint8_t *write_ptr = message;
    while (expected_size > 0)
    {
      yield_if_no_data();
      *write_ptr = uart_recv(UART0);
      printf("Received byte: %02x\n", *write_ptr); // Debug output
      *write_ptr++;
      expected_size--;
    }
    *write_ptr = '\0';
    queue_enqueue(incoming_queue, (char*)message);
    // ownership transferred to queue, no need to free()
  }
}



int main() {
    pynq_init();
    uart_init(UART0);
    uart_reset_fifos(UART0);
    switchbox_init();
    switchbox_set_pin(IO_AR0, SWB_UART0_RX);
    switchbox_set_pin(IO_AR1, SWB_UART0_TX);
    uart_send(UART0, (uint8_t)0u); // Send a dummy byte to initialize the UART
    uart_send(UART0, (uint8_t)0u);
    uart_send(UART0, (uint8_t)0u);

    uart_send(UART0, (uint8_t)4u); // Send a dummy byte to initialize the UART
    uart_send(UART0, (uint8_t)33); // Send a dummy byte to initialize the UART
    uart_send(UART0, (uint8_t)33);
    uart_send(UART0, (uint8_t)33);
    uart_send(UART0, (uint8_t)33);

    sleep(10);
    queue_t *outgoing_queue = queue_create();
    queue_t *incoming_queue = queue_create();
    pthread_t outgoing_thread, incoming_thread;
    pthread_create(&outgoing_thread, NULL, outgoing_networking_handler, outgoing_queue);
    pthread_create(&incoming_thread, NULL, incoming_networking_handler, incoming_queue);
    int testnum = 0;
    printf("Starting test loop...\n");
    while(1) {
      testnum++;
      // end loop after 1000 iterations
      if (testnum > 100) break;
      char* buffer = malloc(BUFFER_SIZE + 1);
      json_serialize(buffer, "iiis", "found", testnum , "x", testnum, "y", 20, "color", "red");
      printf("%s", (buffer));
      queue_enqueue(outgoing_queue, buffer);
      // lose ownership of buffer, queue now owns it
      //printf("%d", (int)strlen(buffer));
      
      if (queue_is_empty(incoming_queue)) {
        printf("No data received\n");
      } else { 
        char *dequeued = queue_dequeue(incoming_queue);
        printf("Data: %s\n", dequeued);
        printf("Data length: %d\n", (int)strlen(dequeued));
        free(dequeued); // Free the dequeued message
      }
      sleep(3); // Sleep for 1 second to simulate time between messages

    }
    pthread_join(outgoing_thread, NULL);
    pthread_join(incoming_thread, NULL);
    uart_destroy(UART0);
    queue_destroy(outgoing_queue);
    queue_destroy(incoming_queue);
    switchbox_destroy();
    pynq_destroy();
    return EXIT_SUCCESS;
}
