/*
 * Relay.cpp
 *
 *  Created on: Dec 12, 2022
 *      Author: tylen
 */

#include <Relay.h>

Relay::Relay () : r (0, 27, DigitalIoPin::output)
{
  r.write (false);
}

Relay::~Relay ()
{}

void
Relay::open()
{
  r.write(true);
}

void
Relay::close()
{
  r.write(false);
}
