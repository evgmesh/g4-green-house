/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "common_values.h"
#include "tasks/green-house_tasks.h"

int
main (void)
{
  /* Generic Initialization */
  SystemCoreClockUpdate ();
  Board_Init ();

  heap_monitor_setup ();

  retarget_init();
  
  xTaskCreate (vMbsensorsTask, "vMbsensorsTask", mbsensorsTASK_STACKSIZE, NULL, 
               mbsensorsTASK_PRIORITY, EMPTY_TASK_HANDLE);

  xTaskCreate (vDisplayTask, "LCD", displayTASK_STACKSIZE, NULL,
               (tskIDLE_PRIORITY + 1UL), EMPTY_TASK_HANDLE);

  xTaskCreate (vButtonTask, "Rotary", buttonTASK_STACKSIZE, NULL,
               buttonTASK_PRIORITY, EMPTY_TASK_HANDLE);

  xTaskCreate (vRelayTask, "Relay", relayTASK_STACKSIZE, NULL,
               relayTASK_PRIORITY, EMPTY_TASK_HANDLE);

  xTaskCreate (vMQTTTask, "MQTTTask", mqttTASK_STACKSIZE, NULL,
               mqttTASK_PRIORITY, EMPTY_TASK_HANDLE);

  /* Start the scheduler */
  vTaskStartScheduler ();
  return 0;
}
