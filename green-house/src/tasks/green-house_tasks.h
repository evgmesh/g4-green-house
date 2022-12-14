/*
 * green-house_tasks.h
 *
 *  Created on: Dec 9, 2022
 *      Author: tylen
 */

#ifndef GREEN_HOUSE_TASKS_H_
#define GREEN_HOUSE_TASKS_H_

#include "EEPROMWrapper.h"
#include "Menu/MenuObj.h"
#include "Relay.h"
#include "common_values.h"

#define EMPTY_TASK_HANDLE (TaskHandle_t *)NULL

/* DISPLAY TASK DEFINES */
#define displayTASK_STACKSIZE (configMINIMAL_STACK_SIZE * 10)
#define displayTASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
void vDisplayTask (void *pvParams);

/* RELAY TASK DEFINES */
#define relayTASK_STACKSIZE (configMINIMAL_STACK_SIZE + 128)
#define relayTASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
void vRelayTask (void *pvParams);

/* MODBUS SENSORS TASK DEFINES */
#define mbsensorsTASK_STACKSIZE (configMINIMAL_STACK_SIZE * 10)
#define mbsensorsTASK_PRIORITY (tskIDLE_PRIORITY + 2UL)
void vMbsensorsTask (void *pvParams);

#endif /* GREEN_HOUSE_TASKS_H_ */
