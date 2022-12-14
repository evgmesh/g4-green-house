/*
 * common_values.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: tylen
 */

#include "common_values.h"

/* global variables */
QueueHandle_t queue;
QueueHandle_t set_point_queue;
QueueHandle_t sensors_q;
extern GH_DATA gh_common = { 0, 0, 0, 0, false };

void
create_all_queues()
{
  queue = xQueueCreate (100, sizeof (RotaryAction));
  set_point_queue = xQueueCreate (1, sizeof (uint32_t));
  sensors_q = xQueueCreate(1, sizeof(GH_DATA));
}
