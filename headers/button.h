/***************** Header *********************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: button.h
 * PROJECT: Elevator project
 * DESCRIPTION: Header file for button driver handling SW1 input and events
 *
 * 
 * 
 */

#ifndef BUTTON_H_
#define BUTTON_H_

/***************** Includes *******************/
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "systick_frt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "glob_def.h"

/***************** Defines ********************/
// GPIO Port F unlock value
#define PORTF_LOCK 0x4C4F434B

// Switch 1 pin mask (PF4)
#define SW1 0x10

/***************** Functions ******************/
/************************************
 * Input   : xTimer - Timer handle
 * Output  : None
 * Function: Timer callback to handle SW1 button press timeout
 ************************************/
void button_timer_callback(TimerHandle_t xTimer);

/************************************
 * Input   : pvParameters - FreeRTOS task parameters
 * Output  : None
 * Function: Task that reads SW1 button, debounces input, and sends button events
 ************************************/
void sw1_task(void* pvParameters);

/************************************
 * Input   : None
 * Output  : None
 * Function: Initialize GPIO Port F for SW1 button input with pull-up resistor
 ************************************/
void init_button();

#endif /* BUTTON_H_ */
