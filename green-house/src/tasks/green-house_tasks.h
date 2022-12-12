/*
 * green-house_tasks.h
 *
 *  Created on: Dec 9, 2022
 *      Author: tylen
 */

#ifndef GREEN_HOUSE_TASKS_H_
#define GREEN_HOUSE_TASKS_H_

#include "common_values.h"
#include "Menu/MenuObj.h"
extern "C"
{
#include "task_mqtt_demo.h"
}

#define EMPTY_TASK_HANDLE (TaskHandle_t *)NULL

/* BUTTON TASK DEFINES */
#define buttonTASK_STACKSIZE (configMINIMAL_STACK_SIZE + 256)
#define buttonTASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
void vButtonTask (void *pvParams);

/* DISPLAY TASK DEFINES */
#define displayTASK_STACKSIZE (configMINIMAL_STACK_SIZE * 4)
#define displayTASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
void vDisplayTask (void *pvParams);

#endif /* GREEN_HOUSE_TASKS_H_ */
