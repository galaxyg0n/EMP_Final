/***************** Header *********************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: master_control.h
 * PROJECT: Assignment_3
 * DESCRIPTION: Header for master control logic for elevator task
 */

#ifndef HEADERS_MASTER_CONTROL_H_
#define HEADERS_MASTER_CONTROL_H_

/***************** Includes *******************/
#include <stdint.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"

#include "lcd.h"
#include "leds.h"
#include "matrix.h"
#include "glob_def.h"
#include "elevator.h"
#include "potentiometer.h"

/***************** Defines ********************/
#define TRIPS_TO_BREAK 4

/***************** Functions ******************/
void master_control_task(void* pvParameters);
/************************************
*Input   : Pointer to parameters (not used)
*Output  : None
*Function: Task managing elevator state machine
************************************/

#endif /* HEADERS_MASTER_CONTROL_H_ */
