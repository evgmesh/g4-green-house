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

  retarget_init ();

  create_all_queues ();

  gh_common = { GH_DATA_RESET };

  xTaskCreate (vMbsensorsTask, "vMbsensorsTask", mbsensorsTASK_STACKSIZE,
               GH_COMMON_PARAM, mbsensorsTASK_PRIORITY, EMPTY_TASK_HANDLE);

  xTaskCreate (vDisplayTask, "LCD", displayTASK_STACKSIZE, GH_COMMON_PARAM,
               displayTASK_PRIORITY, EMPTY_TASK_HANDLE);

  xTaskCreate (vRelayTask, "Relay", relayTASK_STACKSIZE, GH_COMMON_PARAM,
               relayTASK_PRIORITY, EMPTY_TASK_HANDLE);

  xTaskCreate (vMQTTTask, "MQTTTask", mqttTASK_STACKSIZE, GH_COMMON_PARAM,
               mqttTASK_PRIORITY, EMPTY_TASK_HANDLE);

  /* Start the scheduler */
  vTaskStartScheduler ();
  return 0;
}
