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
  set_point_queue = xQueueCreate (1, sizeof (uint32_t));
  Relay valve (&set_point_queue);
  while (true)
    {
      if (!valve.peekForSetPoint ())
        {
          vTaskDelay (1000);
        }
    }
}
