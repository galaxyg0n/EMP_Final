/*
 * main.h
 *
 *  Created on: 5 May 2025
 *      Author: mathi
 */

#ifndef HEADERS_MAIN_H_
#define HEADERS_MAIN_H_

//Standard libs.
#include <stdint.h>
//FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"
//Program specific
#include "glob_def.h"
#include "button.h"
#include "lcd.h"
#include "matrix.h"
#include "uart.h"
#include "elevator.h"
#include "leds.h"
#include "rot_encoder.h"
#include "potentiometer.h"
#include "master_control.h"
#include "tm4c123gh6pm.h"

//Defines
#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define LARGE_TASK_STACK_SIZE 256

#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3
#define VERY_HIGH_PRIO 4

#define UART_QUEUE_LEN 20
#define MATRIX_QUEUE_LEN 10
#define LCD_QUEUE_LEN 32
#define SW1_E_QUEUE_LEN 8


#endif /* HEADERS_MAIN_H_ */
