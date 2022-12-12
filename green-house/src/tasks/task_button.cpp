/*
 * button_task.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: tylen
 */

#include "green-house_tasks.h"

Rotary *rot;

extern "C"
{
  void
  PIN_INT0_IRQHandler (void)
  {
    portEND_SWITCHING_ISR (rot->isr ());
  }
}

void
vButtonTask (void *pvParams)
{
  Rotary rotor (&queue);
  rot = &rotor;
  queue = xQueueCreate (50, sizeof (int));
  vQueueAddToRegistry (queue, "ButtonsQueue");

  /* Make DEBUGOUT work - include retarget_uart*/
  retarget_init ();

  int buffer;
  while (true)
    {
      if (xQueueReceive (queue, &buffer, (TickType_t)5000))
        {
          DEBUGOUT ("%d\r\n", buffer);
//          vTaskDelay (50);
        }
    }
}
