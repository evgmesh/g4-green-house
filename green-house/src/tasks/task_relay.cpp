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
  GH_DATA *global = static_cast<GH_DATA *> (pvParams);
  Relay valve;
  GH_DATA gh_data_relay = { GH_DATA_RESET };
  TickType_t last_opening = 0;
  while (true)
    {
      //xQueueReceive (gh_data_queue, &gh_data_relay, 60000);

      if ((global->co2_val < (float)global->set_point) && (xTaskGetTickCount()) - last_opening > 10000)
        {
    	  last_opening = xTaskGetTickCount();
          valve.open ();
          global->valve_open = true;
        }
      else if((global->co2_val >= (float)global->set_point) && (xTaskGetTickCount()) - last_opening > 1000)
        {
    	  last_opening = xTaskGetTickCount();
          valve.close ();
          global->valve_open = false;
        }
      vTaskDelay(100);
    }
}
