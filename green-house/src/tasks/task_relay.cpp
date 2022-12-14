/*
 * task_relay.cpp
 *
 *  Created on: Dec 12, 2022
 *      Author: tylen
 */

#include "green-house_tasks.h"
#include "Rotary.h"

void
vRelayTask (void *pvParams)
{
  GH_DATA *global = static_cast<GH_DATA *> (pvParams);
  Relay valve;
  GH_DATA gh_data_relay = { GH_DATA_RESET };
  TickType_t last_opening = 0, last_closing = 0;
  RotaryAction message = ROTARY_ACTION::ROTARY_IDLE;
  while (true)
    {
      // xQueueReceive (gh_data_queue, &gh_data_relay, 60000);

      if ((global->co2_val + 50 < (float)global->set_point)
          && (xTaskGetTickCount ()) - last_opening > 30000)
        {          last_opening = xTaskGetTickCount ();
          valve.open ();
          global->valve_open = true;
          xQueueSendToBack(action_q, (void*)&message, 0);
          xQueueSendToBack(gh_data_queue, (void*)&global, 0);
          vTaskDelay(2000);
          valve.close ();
          global->valve_open = false;
          xQueueSendToBack(action_q, (void*)&message, 0);
          xQueueSendToBack(gh_data_queue, (void*)&global, 0);
        }
//      else if ((global->co2_val >= (float)global->set_point - 100) && (xTaskGetTickCount ()) - last_closing > 2000)
//        {
//          last_closing = xTaskGetTickCount ();
//          valve.close ();
//          global->valve_open = false;
//        }
      vTaskDelay (100);
    }
}
