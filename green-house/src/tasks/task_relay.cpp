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
  GH_DATA *global = static_cast<GH_DATA*>(pvParams);
  Relay valve;
  GH_DATA gh_data_relay = GH_DATA_RESET;
  while (true)
    {
      xQueueReceive (gh_data_queue, &gh_data_relay, 1000);

      if (gh_data_relay.co2_val < (float)gh_data_relay.set_point)
      {
        valve.open ();
        global->valve_open = true;
      }else
      {
        valve.close ();
        global->valve_open = false;
      }
    }
}
