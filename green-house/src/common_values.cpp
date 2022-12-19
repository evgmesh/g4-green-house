/*
 * common_values.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: tylen
 */

#include "common_values.h"

/* global variables */
QueueHandle_t action_q;
QueueHandle_t network_q;
SemaphoreHandle_t publish_signal;
SemaphoreHandle_t sensors_ready;
GH_DATA gh_common;

void
create_all_queues ()
{
  action_q = xQueueCreate (100, sizeof (RotaryAction));
  network_q = xQueueCreate(1, sizeof(ND));
  publish_signal = xSemaphoreCreateCounting (5, 0);
  sensors_ready = xSemaphoreCreateBinary ();
}
