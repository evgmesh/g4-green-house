/*
 * task_display.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: tylen
 */
#include "green-house_tasks.h"
#include "Menu/MenuObj.h"
#include "Counter.h"

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
  retarget_init ();
  DigitalIoPin relay (0, 27, DigitalIoPin::output);
  ModbusMaster node3 (240);
  node3.begin (9600);
  node3.idle (idle_delay);
  ModbusRegister RH (&node3, 256, true);
  relay.write (0);
  LiquidCrystal *lcd = createLCD ();
  Counter<uint16_t> ppm(150, 1024, 20);
  MenuObj menu(lcd, &ppm);

  int rotary_action;
  while (true)
    {
      if (xQueueReceive (queue, &rotary_action, (TickType_t)5000))
        {
        }
      vTaskDelay (2000);
    }
}
