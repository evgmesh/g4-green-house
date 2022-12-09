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

/* std includes */
#include <cstring>

/* Common structures */
typedef struct _green_house
{
  uint32_t co_two_level;
} GH_DATA;

/* Common defines */
#define TICKRATE_HZ (10) /* 10 ticks per second */

/* global variables */
extern QueueHandle_t queue;
extern Rotary *rot;

#endif /* COMMON_VALUES_H_ */
