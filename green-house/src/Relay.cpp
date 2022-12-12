/*
 * Relay.cpp
 *
 *  Created on: Dec 12, 2022
 *      Author: tylen
 */

#include <Relay.h>

Relay::Relay (QueueHandle_t *sp_queue) : r (0, 27, DigitalIoPin::output)
{
  r.write (false);
  set_point = 0;
  q = sp_queue;
}

bool
Relay::peekForSetPoint (void)
{
  uint32_t new_sp;
  BaseType_t peek = xQueuePeek (*q, &new_sp, (TickType_t)5000);
  if (peek && (new_sp != set_point))
    {
      set_point = new_sp;
      return true;
    }
  return false;
}

Relay::~Relay ()
{
  // TODO Auto-generated destructor stub
}
