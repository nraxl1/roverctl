#include <pthread.h>

typedef struct RGB
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} rgb_t;

typedef struct SensorFrame
{
  pthread_mutex_t lock; // mutex to protect access to the sensor frame
  float distance1;
  float distance2;
  float distance3;
  rgb_t color1;
  rgb_t color2;
  uint8_t confidence; // increased every time the sensor thread finds the frame unflushed and averages it with another frame. set to 0 on flush.
} sensor_frame_t;

#ifdef TEST_BUILD

void *handle_sensors(void *sensor_frame)
{
  sensor_frame_t *frame = (sensor_frame_t *)sensor_frame;
  pthread_mutex_t *sensor_mutex = &frame->lock;
  while (1)
  {
    pthread_mutex_lock(sensor_mutex);
    if (sensor_frame->confidence == 0)
    {
      // get the next set of sensor values
      // populate the sensor_frame (set confidence to 1)
    }
    else
    {
      // @warning: branch must yield in less than 100ms (standard quantum) or control thread will be left without data for 1 cycle, interruption while interfacing with the sensors will generate garbage data.
      // get as many sets of sensor values as you can
      // update the sensor frame with the new data points
      // increment the confidence value
    }
    pthread_mutex_unlock(sensor_mutex);
    pthread_yield();
  }
  return 0;
}

#else

void *handle_sensors(void *sensor_frame)
{
  sensor_frame_t *frame = (sensor_frame_t *)sensor_frame;
  pthread_mutex_t *sensor_mutex = &frame->lock;
  while (1)
  {
    pthread_mutex_lock(sensor_mutex);
    if (sensor_frame->confidence == 0)
    {
      // get the next set of sensor values
      // populate the sensor_frame (set confidence to 1)
    }
    else
    {
      // @warning: branch must yield in less than 100ms (standard quantum) or control thread will be left without data for 1 cycle, interruption while interfacing with the sensors will generate garbage data.
      // get as many sets of sensor values as you can
      // update the sensor frame with the new data points
      // increment the confidence value
    }
    pthread_mutex_unlock(sensor_mutex);
    pthread_yield();
  }
  return 0;
}

#endif /* TEST_BUILD */
