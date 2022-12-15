/*
 * common_values.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: tylen
 */

#include "common_values.h"

/* global variables */
QueueHandle_t action_q;
QueueHandle_t gh_data_queue;
GH_DATA gh_common;

void
create_all_queues ()
{
  action_q = xQueueCreate (100, sizeof (RotaryAction));
  gh_data_queue = xQueueCreate (4, sizeof (GH_DATA));
}
