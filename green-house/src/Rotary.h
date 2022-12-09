/**
 * @file Rotary.h
 * @author tylen
 * @brief
 * @version 0.1
 * @date 2022-12-09
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef ROTARY_H
#define ROTARY_H

#include "DigitalIoPin.h"
#include "FreeRTOS.h"
#include "task.h"
#include "chip.h"

enum
{
  SIGNAL_ROTATE_CLO,
  SIGNAL_ROTATE_CCLO,
  SIGNAL_PRESS
};

class Rotary
{
public:
  Rotary ();
  ~Rotary ();
  portBASE_TYPE isr ();

private:
  DigitalIoPin signal[3] = { { 0, 6, DigitalIoPin::input, true },
                             { 0, 5, DigitalIoPin::input, true },
                             { 1, 8, DigitalIoPin::input, true } };

};

#endif /* ROTARY_H */
