#include <pthread>
#include "mutex_queue.h"

#define TRAVERSE 0
#define IDENTIFY 1

typedef struct
{
  mutex_queue_t *outgoing;
  mutex_queue_t *incoming;
  sensor_frame_t *sensor_frame;
} control_args_t;

#ifdef TEST_BUILD // This snippet is only for testing purposes, it should not be included in the final build
// If a test shouldnt run this part
void *control(void *args)
{
  control_args_t *ctrl_args = (control_args_t *)args;
  mutex_queue_t *outgoing = ctrl_args->outgoing;
  mutex_queue_t *incoming = ctrl_args->incoming;
  sensor_frame_t *sensor_frame = ctrl_args->sensor_frame;

  uint opmode = TRAVERSE;
  // list of found objects and their locations required here
  int location_x = 0;
  int location_y = 0;
  int angle_degrees = 0; // angle from positive y-axis (towards cartesian quadrant 1)
  while (1)
  {
    // underlying while loops are not to exceed 100ms (standard quantum) of runtime before yielding
    // otherwise the thread might get interrupted while accessing FPGA memory mappings, which should not lead to undefined behavior but is not advisable either.
    while (opmode == TRAVERSE)
    {
      // decide where to go
      // step towards going there
      // check for objects
      // change to IDENTIFY operation mode if needed
      pthread_yield();
    }
    while (opmode == IDENTIFTY)
    {
      // approach while checking ground
      // if another object found, interrupt process and identify it
      // when approached, check sensor data against thresholds to identify it
      // use circular motions to ID cliff/border and different movement flowchart to id small rock/large rock
      // change to TRAVERSE operation mode when object identified
      pthread_yield();
    }
  }

#else

void *control(void *args)
{
  control_args_t *ctrl_args = (control_args_t *)args;
  mutex_queue_t *outgoing = ctrl_args->outgoing;
  mutex_queue_t *incoming = ctrl_args->incoming;
  sensor_frame_t *sensor_frame = ctrl_args->sensor_frame;

  uint opmode = TRAVERSE;
  // list of found objects and their locations required here
  int location_x = 0;
  int location_y = 0;
  int angle_degrees = 0; // angle from positive y-axis (towards cartesian quadrant 1)
  while (1)
  {
    // underlying while loops are not to exceed 100ms (standard quantum) of runtime before yielding
    // otherwise the thread might get interrupted while accessing FPGA memory mappings, which should not lead to undefined behavior but is not advisable either.
    while (opmode == TRAVERSE)
    {
      // decide where to go
      // step towards going there
      // check for objects
      // change to IDENTIFY operation mode if needed
      pthread_yield();
    }
    while (opmode == IDENTIFTY)
    {
      // approach while checking ground
      // if another object found, interrupt process and identify it
      // when approached, check sensor data against thresholds to identify it
      // use circular motions to ID cliff/border and different movement flowchart to id small rock/large rock
      // change to TRAVERSE operation mode when object identified
      pthread_yield();
    }
  }

#endif /* TEST_BUILD */