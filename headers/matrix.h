/***************** Header *********************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: matrix.h
 * PROJECT: Assignment_3
 * DESCRIPTION: Header file for matrix keypad driver
 */

#ifndef HEADERS_MATRIX_H_
#define HEADERS_MATRIX_H_

/***************** Includes *******************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "glob_def.h"
#include "uart.h"
#include "lcd.h"

#include "tm4c123gh6pm.h"

/***************** Defines ********************/
#define COL_1 0x10  // PA4
#define COL_2 0x08  // PA3
#define COL_3 0x04  // PA2

#define ROW_1 0x03  // PE0
#define ROW_2 0x02  // PE1
#define ROW_3 0x01  // PE2
#define ROW_4 0x00  // PE3

#define KEYPAD_QUEUE_LEN 20
#define KEYPAD_QUEUE_ITEM sizeof(KEYPAD_STRUCT)

/***************** Const. *********************/
/***************** Variables ******************/
/***************** Functions ******************/
void init_matrix(void);
/************************************
*Input   : None
*Output  : None
*Function: Initializes GPIO ports for matrix keypad
************************************/

void sweep_keypad_task(void *pvParameters);
/************************************
*Input   : Task parameters (unused)
*Output  : None
*Function: FreeRTOS task that continuously scans the keypad
************************************/

bool keypad_queue_put(KEYPAD_STRUCT queueStruct);
/************************************
*Input   : Structure containing the pressed key
*Output  : true if queued successfully, false otherwise
*Function: Sends a keypress to the keypad queue
************************************/

bool keypad_queue_get(KEYPAD_STRUCT *returnStruct);
/************************************
*Input   : Pointer to receive keypress
*Output  : true if key was received, false otherwise
*Function: Retrieves a keypress from the keypad queue
************************************/

#endif /* HEADERS_MATRIX_H_ */
