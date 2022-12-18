/*
 * Relay.cpp
 *
 *  Created on: Dec 12, 2022
 *      Author: tylen
 */

#include <Relay.h>

Relay::Relay (GH_DATA *data, TickType_t keep_valve_closed)
    : r (0, 27, DigitalIoPin::output)
{
  data_loc = data;
  last_opening = 0;
  keep_closed = keep_valve_closed;
  r.write (false);
}

Relay::~Relay () {}

bool
Relay::peekForSetPointClosure ()
{
  if ((data_loc->co2_val + 50 < (float)data_loc->set_point)
      && (xTaskGetTickCount ()) - last_opening > keep_closed)
    {
      last_opening = xTaskGetTickCount ();
      return true;
    }
  return false;
}

void
Relay::open ()
{
  data_loc->valve_open = true;
  r.write (true);
}

void
Relay::close ()
{
  data_loc->valve_open = false;
  r.write (false);
}
