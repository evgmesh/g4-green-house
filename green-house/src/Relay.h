/*
 * Relay.h
 *
 *  Created on: Dec 12, 2022
 *      Author: tylen
 */

#ifndef RELAY_H_
#define RELAY_H_

#include "common_values.h"
class Relay
{
public:
  Relay (GH_DATA *data, TickType_t keep_valve_closed, float threshold = 10);
  void open ();
  void close ();
  bool peekForSetPointClosure ();
  virtual ~Relay ();

private:
  DigitalIoPin r;
  GH_DATA *data_loc;
  float th;
  TickType_t last_opening;
  TickType_t keep_closed;
};

#endif /* RELAY_H_ */
