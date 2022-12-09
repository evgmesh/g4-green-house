/*
 * task_mqtt_demo.h
 *
 *  Created on: Dec 9, 2022
 *      Author: tylen
 */

#ifndef TASKS_TASK_MQTT_DEMO_H_
#define TASKS_TASK_MQTT_DEMO_H_

#define democonfigDEMO_STACKSIZE (configMINIMAL_STACK_SIZE + 256)

/**
 * @brief The task used to demonstrate the MQTT API.
 *
 * @param[in] pvParameters Parameters as passed at the time of task creation.
 * Not used in this example.
 */
void vMQTTDemoTask (void *pvParameters);

#endif /* TASKS_TASK_MQTT_DEMO_H_ */
