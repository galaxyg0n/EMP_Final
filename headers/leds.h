/***************** Header *********************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: leds.h
 * PROJECT   : Elevator project
 * DESCRIPTION: LED's function library header file
 */
#ifndef HEADERS_LEDS_H_
#define HEADERS_LEDS_H_

/***************** Includes *******************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "glob_def.h"
#include "event_groups.h"
#include "tm4c123gh6pm.h"

/***************** Defines *********************/
#define LED_R    0x02    // Red LED (Port F pin 1)
#define LED_Y    0x04    // Yellow LED (Port F pin 2)
#define LED_G    0x08    // Green LED (Port F pin 3)
#define CONST_G  0x10    // Event bit for special green LED condition

/***************** Functions *******************/
void init_leds(void);
/************************************
* Initializes the GPIO pins for LEDs as outputs and disables them initially
************************************/

void led_task(void* pvParameters);
/************************************
* FreeRTOS task that controls LED blinking/status based on event group bits
************************************/

#endif /* HEADERS_LEDS_H_ */
