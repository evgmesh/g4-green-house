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
  Relay (QueueHandle_t *sp_queue);
  bool peekForSetPoint (void);
  virtual ~Relay ();

private:
  DigitalIoPin r;
  uint32_t set_point;
  uint32_t current_value;
  QueueHandle_t *q;
};

#endif /* RELAY_H_ */
