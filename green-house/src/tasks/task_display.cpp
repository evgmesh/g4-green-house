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
  retarget_init ();
  DigitalIoPin relay (0, 27, DigitalIoPin::output); // CO2 relay
  relay.write (0);

#if 0 /* HUMIDITY */
		ModbusMaster node3(241); // Create modbus object that connects to slave id 241 (HMP60)
		node3.begin(9600); // all nodes must operate at the same speed!
		node3.idle(idle_delay); // idle function is called while waiting for reply from slave
		ModbusRegister RH(&node3, 256, true);
#endif
  /* CO2
   *
   */
  ModbusMaster node3 (
      240); // Create modbus object that connects to slave id 241 (HMP60)
  node3.begin (9600);      // all nodes must operate at the same speed!
  node3.idle (idle_delay); // idle function is called while waiting for reply
                           // from slave
  ModbusRegister RH (&node3, 256, true);

  LiquidCrystal *lcd = createLCD ();

  // Print a message to the LCD.
  lcd->print ("MQTT_FreeRTOS");

  while (true)
    {
      float rh;
      char buffer[32];

      vTaskDelay (2000);

      rh = RH.read ();
      snprintf (buffer, 32, "CO2=%5.1f", rh);
      printf ("%s\n", buffer);
      lcd->setCursor (0, 1);
      // Print a message to the LCD.
      lcd->print (buffer);
    }
}
