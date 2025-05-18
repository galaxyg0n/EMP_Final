/***************** Header *********************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: main.h
 * PROJECT: Elevator
 * DESCRIPTION: Main module header file. Includes standard libraries,
 *              FreeRTOS components, project-specific modules, and system
 *              configuration macros.
 */

#ifndef HEADERS_MAIN_H_
#define HEADERS_MAIN_H_

/***************** Includes *******************/
#include <stdint.h>                 

// FreeRTOS headers
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"

// Project-specific modules
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

// Device-specific header
#include "tm4c123gh6pm.h"

/***************** Defines ********************/
#define USERTASK_STACK_SIZE      configMINIMAL_STACK_SIZE  // Base stack size for small tasks
#define LARGE_TASK_STACK_SIZE    256                       // Larger stack size for tasks needing more memory

// Task priorities
#define IDLE_PRIO                0
#define LOW_PRIO                 1
#define MED_PRIO                 2
#define HIGH_PRIO                3
#define VERY_HIGH_PRIO           4

// Queue lengths
#define UART_QUEUE_LEN           20
#define MATRIX_QUEUE_LEN         10
#define LCD_QUEUE_LEN            32
#define SW1_E_QUEUE_LEN          8

/***************** Const. *********************/
/***************** Variables ******************/
/***************** Functions ******************/

#endif /* HEADERS_MAIN_H_ */
