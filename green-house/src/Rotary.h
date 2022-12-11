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

#include "Counter.h"
#include "DigitalIoPin.h"
#include "FreeRTOS.h"
#include "chip.h"
#include "common_values.h"
#include "queue.h"
#include "task.h"
#include <assert.h>


enum
{
  SIGNAL_ROTATE_CLO,
  SIGNAL_ROTATE_CCLO,
  SIGNAL_PRESS
};

class Rotary
{
public:
  Rotary (QueueHandle_t *level_q);
  ~Rotary ();
  portBASE_TYPE isr ();

private:
  DigitalIoPin signal[3] = { { 0, 6, DigitalIoPin::input, true },
                             { 0, 5, DigitalIoPin::input, true },
                             { 1, 8, DigitalIoPin::input, true } };
  Counter level;
  QueueHandle_t *_level_q;
};

#endif /* ROTARY_H */
