/*
 * common_values.h
 *
 *  Created on: Dec 4, 2022
 *      Author: tylen
 */

#ifndef COMMON_VALUES_H_
#define COMMON_VALUES_H_

/* LPC includes */
#include "board.h"
#include "chip.h"
#include <cr_section_macros.h>

/* FreeRTOS inlcudes */
#include "FreeRTOS.h"
#include "heap_lock_monitor.h"
#include "retarget_uart.h"
#include "semphr.h"
#include "task.h"

/* Greenhouse includes */
#include "DigitalIoPin.h"
#include "EEPROMWrapper.h"
#include "LiquidCrystal.h"
#include "ModbusRegister.h"
#include "Rotary.h"
#include "mqtt.h"

/* std includes */
#include <cstring>

/* Common structures */
typedef struct _green_house
{
  float co2_val;
  float rhum_val;
  float temp_val;
  uint16_t set_point;
  bool valve_open;
} GH_DATA;

/* Common defines */
#define TICKRATE_HZ (10) /* 10 ticks per second */
#define ARDUINO_SIM 1
typedef uint8_t RotaryAction;
#define GH_COMMON_PARAM (void *)&gh_common

#define GH_DATA_RESET   0, 0, 0, 0, false


/* global variables */
extern QueueHandle_t action_q;
extern QueueHandle_t gh_data_queue;
extern Rotary *rot;
extern GH_DATA gh_common;

/* global functions */
void create_all_queues ();

#endif /* COMMON_VALUES_H_ */
