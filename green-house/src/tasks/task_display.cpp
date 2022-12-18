/*
 * task_display.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: tylen
 */
#include "green-house_tasks.h"

static void
idle_delay ()
{
  vTaskDelay (1);
}

static LiquidCrystal *
createLCD ()
{
  // Configure pins
  DigitalIoPin *rs = new DigitalIoPin (0, 29, DigitalIoPin::output);
  DigitalIoPin *en = new DigitalIoPin (0, 9, DigitalIoPin::output);
  DigitalIoPin *d4 = new DigitalIoPin (0, 10, DigitalIoPin::output);
  DigitalIoPin *d5 = new DigitalIoPin (0, 16, DigitalIoPin::output);
  DigitalIoPin *d6 = new DigitalIoPin (1, 3, DigitalIoPin::output);
  DigitalIoPin *d7 = new DigitalIoPin (0, 0, DigitalIoPin::output);
  // Create new display object
  LiquidCrystal *lcd = new LiquidCrystal (rs, en, d4, d5, d6, d7);
  // configure display geometry
  lcd->begin (16, 2);
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd->setCursor (0, 0);
  return lcd;
}

void
vDisplayTask (void *pvParams)
{
  GH_DATA *gh_data_display = static_cast<GH_DATA *> (pvParams);
  LiquidCrystal *lcd = createLCD ();
  EEPROM_Wrapper mem;
  MenuObj menu (lcd, new Counter<uint16_t> (200, 9999, 10), &mem,
                gh_data_display, &publish_signal);

  RotaryAction rotary_action = ROTARY_ACTION::ROTARY_IDLE;
  while (true)
    {
      if (!xQueueReceive (action_q, &rotary_action, 3000))
        {
          rotary_action = ROTARY_ACTION::ROTARY_IDLE;
        }
      menu.HandleRotaryAction (rotary_action);
    }
}
