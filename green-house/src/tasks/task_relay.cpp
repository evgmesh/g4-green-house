/*
 * task_relay.cpp
 *
 *  Created on: Dec 12, 2022
 *      Author: tylen
 */

#include "Rotary.h"
#include "green-house_tasks.h"

void
vRelayTask (void *pvParams)
{
  Relay valve (static_cast<GH_DATA *> (pvParams), 30000);
  TickType_t last_opening = 0;
  RotaryAction message = ROTARY_ACTION::ROTARY_IDLE;
  while (true)
    {
      if (valve.peekForSetPointClosure ())
        {
          valve.open ();
          xQueueSendToBack (action_q, (void *)&message, 0);
          xSemaphoreGive (publish_signal);
          vTaskDelay (10000);
          valve.close ();
          xQueueSendToBack (action_q, (void *)&message, 0);
          xSemaphoreGive (publish_signal);
        }
    }
}
