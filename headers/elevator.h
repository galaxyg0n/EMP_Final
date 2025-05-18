/***************** Header *********************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: elevator.h
 * PROJECT: Elevator project
 * DESCRIPTION: Header file for elevator control task and related functions
 *
 *
 *
 */

/***************** Header Guard *********************/
#ifndef HEADERS_ELEVATOR_H_
#define HEADERS_ELEVATOR_H_

/***************** Includes *******************/
#include <stdio.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"
#include "lcd.h"
#include "glob_def.h"
#include "button.h"
#include "master_control.h"
#include "leds.h"
#include "tm4c123gh6pm.h"

/***************** Defines ********************/
#define FLIP 1
#define NO_FLIP 0
#define FORBIDDEN_FLOOR 13

/***************** Functions ******************/
/************************************
* Input   : choice - uint8_t flag to toggle user presence state
* Output  : Returns current user-in-elevator status as uint8_t
* Function: Toggles and returns whether user is inside the elevator
************************************/
uint8_t is_user_in_elevator(uint8_t choice);

/************************************
* Input   : pvParameters - Task parameter (unused)
* Output  : None
* Function: Elevator control FreeRTOS task. Manages elevator states:
*           NO_MOTION, INIT_MOTION, IN_MOTION
************************************/
void elevator_task(void* pvParameters);

#endif /* HEADERS_ELEVATOR_H_ */
