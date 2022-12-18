/*
 * Relay.cpp
 *
 *  Created on: Dec 12, 2022
 *      Author: tylen
 */

#include <Relay.h>

Relay::Relay (GH_DATA *data, TickType_t keep_valve_closed, float threshold)
    : r (0, 27, DigitalIoPin::output)
{
  data_loc = data;
  last_opening = 0;
  keep_closed = keep_valve_closed;
  th = threshold;
  r.write (false);
}

Relay::~Relay () {}

bool
Relay::peekForSetPointClosure ()
{
  if (xSemaphoreTake (sensors_ready, 100)
      && ((data_loc->co2_val + th < (float)data_loc->set_point)
          && (xTaskGetTickCount ()) - last_opening > keep_closed))
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
