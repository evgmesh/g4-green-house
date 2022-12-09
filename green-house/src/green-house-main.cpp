/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "board.h"
#include "chip.h"

#include <cr_section_macros.h>

// TODO: insert other include files here
#include "FreeRTOS.h"
#include "heap_lock_monitor.h"
#include "retarget_uart.h"
#include "semphr.h"
#include "task.h"

#include "DigitalIoPin.h"
#include "EEPROMWrapper.h"
#include "LiquidCrystal.h"
#include "ModbusRegister.h"
#include "Rotary.h"
#include "common_definitions.h"
#include <cstring>
/* Systick interrupt rate */
#define TICKRATE_HZ (10) /* 10 ticks per second */

static volatile int counter;
static DigitalIoPin SW1 (1, 8, DigitalIoPin::input, true);
static DigitalIoPin SW2 (0, 5, DigitalIoPin::input, true);
static DigitalIoPin SW3 (0, 6, DigitalIoPin::input, true);

static DigitalIoPin sw_a2 (1, 8, DigitalIoPin::pullup, true);
static DigitalIoPin sw_a3 (0, 5, DigitalIoPin::input, true);
static DigitalIoPin sw_a4 (0, 6, DigitalIoPin::input, true);
static DigitalIoPin sw_a5 (0, 7, DigitalIoPin::pullup, true);

static QueueHandle_t queue;
Rotary *rot;

extern "C"
{
  void
  PIN_INT0_IRQHandler (void)
  {
    portEND_SWITCHING_ISR (rot->isr ());
  }
}

void interrupt_init (void);

/* The following is required if runtime statistics are to be collected
 * Copy the code to the source file where other you initialize hardware */
extern "C"
{

  void
  vConfigureTimerForRunTimeStats (void)
  {
    Chip_SCT_Init (LPC_SCTSMALL1);
    LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
    LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L (255)
                            | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 +
                                                 // 1), and start timer
  }
}
/* end runtime statictics collection */

static void
idle_delay ()
{
  vTaskDelay (1);
}

void
task_Display (void *params)
{
  (void)params;

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

  DigitalIoPin *rs = new DigitalIoPin (0, 29, DigitalIoPin::output);
  DigitalIoPin *en = new DigitalIoPin (0, 9, DigitalIoPin::output);
  DigitalIoPin *d4 = new DigitalIoPin (0, 10, DigitalIoPin::output);
  DigitalIoPin *d5 = new DigitalIoPin (0, 16, DigitalIoPin::output);
  DigitalIoPin *d6 = new DigitalIoPin (1, 3, DigitalIoPin::output);
  DigitalIoPin *d7 = new DigitalIoPin (0, 0, DigitalIoPin::output);
  LiquidCrystal *lcd = new LiquidCrystal (rs, en, d4, d5, d6, d7);
  // configure display geometry
  lcd->begin (16, 2);
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd->setCursor (0, 0);
  // Print a message to the LCD.
  lcd->print ("MQTT_FreeRTOS");

  while (true)
    {
      float rh;
      char buffer[32];

      vTaskDelay (2000);

      rh = RH.read ();
      snprintf (buffer, 32, "CO2=%5.1f%", rh);
      printf ("%s\n", buffer);
      lcd->setCursor (0, 1);
      // Print a message to the LCD.
      lcd->print (buffer);
    }
}

void
vButtonTask (void *pvParams)
{

  queue = xQueueCreate (50, sizeof (int));
  vQueueAddToRegistry (queue, "ButtonsQueue");
  counter = 10;
  interrupt_init ();

  /* Make DEBUGOUT work - include retarget_uart*/
  retarget_init ();

  int buffer;
  while (true)
    {
      if (xQueueReceive (queue, &buffer, (TickType_t)5000))
        {
          counter += buffer;
          DEBUGOUT ("%d\r\n", counter);
          vTaskDelay (50);
        }
      else
        counter = 10;
    }
}

extern "C"
{
  void vStartSimpleMQTTDemo (void); // ugly - should be in a header
}

int
main (void)
{
  /* Generic Initialization */
  SystemCoreClockUpdate ();
  Board_Init ();

  /* Enable SysTick Timer */
  SysTick_Config (SystemCoreClock / TICKRATE_HZ);

  EEPROM_Wrapper mem;
  GH_DATA house = { 12 };
  Rotary * rotor;
  rot = rotor;

#if 0
  /**************************************** WRITE*********************************************/
  // Try to write a structure
  mem.write_to (EEPROM_ADDRESS, &house, sizeof (GH_DATA));
  // Write string afterwards
  mem.write_to (EEPROM_ADDRESS + sizeof (GH_DATA), "Hello from EEPROM");

  /**************************************** READ
   * ********************************************/
  // Read a structure
  GH_DATA *new_data
      = (GH_DATA *)mem.read_from (EEPROM_ADDRESS, sizeof (GH_DATA));
  // Read a string
  std::string str = mem.str_read_from (EEPROM_ADDRESS + sizeof (GH_DATA), 256);

  DEBUGSTR (str.c_str ()); // Prints "Hello from EEPROM"
#endif

  heap_monitor_setup ();

  //  xTaskCreate (task_Display, "lcd", configMINIMAL_STACK_SIZE * 4, NULL,
  //               (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *)NULL);

  xTaskCreate (vButtonTask, "rotary", configMINIMAL_STACK_SIZE + 128, NULL,
               (tskIDLE_PRIORITY + 1), (TaskHandle_t *)NULL);

  vStartSimpleMQTTDemo ();
  /* Start the scheduler */
  vTaskStartScheduler ();
  return 0;
}

void
interrupt_init (void)
{
  /* Initialize PININT driver */
  Chip_PININT_Init (LPC_GPIO_PIN_INT);

  /* Enable PININT clock */
  Chip_Clock_EnablePeriphClock (SYSCTL_CLOCK_PININT);

  /* Reset the PININT block */
  Chip_SYSCTL_PeriphReset (RESET_PININT);

  /* Configure interrupt channel for the GPIO pin in INMUX block */
  Chip_INMUX_PinIntSel (0, 0, 6);

  /* Configure channel interrupt as edge sensitive and falling edge interrupt*/
  Chip_PININT_ClearIntStatus (LPC_GPIO_PIN_INT, PININTCH (0));
  Chip_PININT_SetPinModeEdge (LPC_GPIO_PIN_INT, PININTCH (0));
  Chip_PININT_EnableIntLow (LPC_GPIO_PIN_INT, PININTCH (0));
  Chip_PININT_DisableIntHigh (LPC_GPIO_PIN_INT, PININTCH (0));

  /* Enable interrupt in the NVIC */
  NVIC_ClearPendingIRQ (PIN_INT0_IRQn);
  NVIC_SetPriority (PIN_INT0_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY + 1);
  NVIC_EnableIRQ (PIN_INT0_IRQn);
}
