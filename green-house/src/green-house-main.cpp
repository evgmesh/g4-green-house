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
#include "mqtt.h"

void vMQTTTask (void *pvParameters) {
	std::string topic("Please, print something!");
	std::string message("Please, print something!");
	mqtt mqtt;
	mqtt.publish(topic, message);

	while(true) {
		vTaskDelay(1000);
	}
}


int
main (void)
{
  /* Generic Initialization */
  SystemCoreClockUpdate ();
  Board_Init ();


  heap_monitor_setup ();

  xTaskCreate (vDisplayTask, "LCD", displayTASK_STACKSIZE, NULL,
               (tskIDLE_PRIORITY + 1UL), EMPTY_TASK_HANDLE);

  xTaskCreate (vButtonTask, "Rotary", buttonTASK_STACKSIZE, NULL,
               buttonTASK_PRIORITY, EMPTY_TASK_HANDLE);

  xTaskCreate (vRelayTask, "Relay", relayTASK_STACKSIZE, NULL,
               relayTASK_PRIORITY, EMPTY_TASK_HANDLE);

  xTaskCreate (vMQTTTask, "MQTTTask", democonfigDEMO_STACKSIZE, NULL,
               displayTASK_PRIORITY, EMPTY_TASK_HANDLE);

  /* Start the scheduler */
  vTaskStartScheduler ();
  return 0;
}
