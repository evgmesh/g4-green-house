/*
 * common_values.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: tylen
 */

#include "common_values.h"

/* global variables */
QueueHandle_t action_q;
SemaphoreHandle_t publish_signal;
SemaphoreHandle_t read_sensors_signal;
GH_DATA gh_common;

void
create_all_queues ()
{
  action_q = xQueueCreate (100, sizeof (RotaryAction));
  publish_signal = xSemaphoreCreateCounting (5, 0);
}
