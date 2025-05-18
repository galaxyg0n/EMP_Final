/***************** Header File ****************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: rot_encoder.h
 * PROJECT: Elevator project
 * DESCRIPTION: Header for rotary encoder driver and task
 *
 *
 *
 *
 */

#ifndef HEADERS_ROT_ENCODER_H_
#define HEADERS_ROT_ENCODER_H_

/***************** Includes *******************/
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "glob_def.h"
#include "small_sprints.h"
#include "lcd.h"
#include "elevator.h"
#include "tm4c123gh6pm.h"
#include "uart.h"

/***************** Defines ********************/
#define DIGI_A              5
#define DIGI_B              6
#define DIGI_P2             7

#define ROTARY_QUEUE_LEN    20
#define ROTARY_QUEUE_ITEM   sizeof(ROT_EVENT)

/***************** Typedefs *******************/
typedef enum {
    SCROLL_UP,
    SCROLL_DOWN,
    BUTTON
} ROT_EVENT;

/***************** Public Function Prototypes ***************/
void rotary_timer_callback(TimerHandle_t xTimer);
/*
 * DESCRIPTION: Callback function for rotary encoder debounce timer
 * PARAMETER:   xTimer - Timer handle triggering the callback
 */

void init_rotary();
/*
 * DESCRIPTION: Initializes rotary encoder GPIO and interrupts
 */

void rotary_task();
/*
 * DESCRIPTION: FreeRTOS task for handling rotary encoder state
 */

bool rotary_queue_put(char c);
/*
 * DESCRIPTION: Puts a character into the rotary encoder queue (if used)
 * PARAMETER:   c - character to enqueue
 * RETURN:      true if successful, false otherwise
 */

#endif /* HEADERS_ROT_ENCODER_H_ */
