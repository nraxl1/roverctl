#include "mutex_queue.h"

#define TRAVERSE 0
#define IDENTIFY 1

int control(mutex_queue_t *outgoing, mutex_queue_t *incoming, pthread_mutex_lock *sensor_mutex, sensor_frame_t *sensor_frame) {
  uint opmode = TRAVERSE;
  // list of found objects and their locations required here
  int location_x = 0;
  int location_y = 0;
  int angle_degrees = 0; // angle from positive y-axis (towards cartesian quadrant 1)
  while (1){
    while (opmode == TRAVERSE){
      // decide where to go 
      // step towards going there
      // check for objects
      // change to IDENTIFY operation mode if needed
    }
    while (opmode == IDENTIFTY){
      // approach while checking ground
      // if another object found, interrupt process and identify it
      // when approached, check sensor data against thresholds to identify it
      // use circular motions to ID cliff/border and different movement flowchart to id small rock/large rock
      // change to TRAVERSE operation mode when object identified
    }
    return 0;
}
