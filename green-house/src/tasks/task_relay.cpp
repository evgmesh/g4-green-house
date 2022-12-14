/*
 * task_relay.cpp
 *
 *  Created on: Dec 12, 2022
 *      Author: tylen
 */

#include "green-house_tasks.h"

void
vRelayTask (void *pvParams)
{
  GH_DATA *sensors_actuators = static_cast<GH_DATA *>(pvParams);
  Relay valve;
  uint32_t point = 0;
  while (true)
    {
      xQueueReceive(set_point_queue, &point, 1000);

      if(sensors_actuators->co2_val < (float)point)
        valve.open();
      else
        valve.close();
    }
}
