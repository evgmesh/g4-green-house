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
  Relay ();
  void open();
  void close();
  virtual ~Relay ();

private:
  DigitalIoPin r;
};

#endif /* RELAY_H_ */
