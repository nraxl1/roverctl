#ifndef SENSOR_H
#define SENSOR_H

#include <pthread.h>

typedef struct RGB
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} rgb_t;

typedef struct SensorFrame
{
  float distance1;
  float distance2;
  float distance3;
  rgb_t color1;
  rgb_t color2;
  uint8_t confidence; /**< @brief increased every time the sensor thread finds the frame unflushed and averages it with another frame. set to 0 on flush. */
} sensor_frame_t;

void *handle_sensors(void *sensor_frame);

#endif /* SENSOR_H */
