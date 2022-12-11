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
#include "chip.h"
#include "queue.h"
#include "task.h"
#include <assert.h>

/* Rotary action enumeration */
enum ROTARY_ACTION{
  ROTARY_CLOCKWISE,
  ROTARY_CCLOCKWISE,
  ROTARY_PRESS
};


class Rotary
{
public:
  Rotary (QueueHandle_t *level_q);
  ~Rotary ();
  portBASE_TYPE rotate_isr ();
  portBASE_TYPE press_isr ();

private:
  DigitalIoPin signal[3] = { { 0, 6, DigitalIoPin::input, true },
                             { 0, 5, DigitalIoPin::input, true },
                             { 1, 8, DigitalIoPin::input, true } };

  QueueHandle_t *_level_q;
  void sendActionAndAssertQueue (uint8_t *data, BaseType_t *const pxHPW);
};

#endif /* ROTARY_H */
