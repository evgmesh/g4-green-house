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

//xQueuePeek reference.
void
vTestTask(void * pvParams)
{
  GH_DATA sen;
  sen.co2_val = 0;
  sen.rhum_val = 0;
  sen.temp_val = 0;
  while (1)
  {
    xQueuePeek(sensors_q, &sen, pdMS_TO_TICKS(5000));

    printf("[TEST] Sensors data:\r\nco2: %.2f\r\nrel hum: %.2f\r\ntemp: %.2f\r\n", sen.co2_val, sen.rhum_val, sen.temp_val);
    vTaskDelay(4000);
  }
}

int
main (void)
{
  /* Generic Initialization */
  SystemCoreClockUpdate ();
  Board_Init ();

  heap_monitor_setup ();

  retarget_init();

  sensors_q = xQueueCreate(1, sizeof(GH_DATA));
  
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

  xTaskCreate (vTestTask, "vTestTask", mqttTASK_STACKSIZE, NULL,
               relayTASK_PRIORITY, EMPTY_TASK_HANDLE);

  /* Start the scheduler */
  vTaskStartScheduler ();
  return 0;
}
