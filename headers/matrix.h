/*
 * matrix.h
 *
 *  Created on: 5 May 2025
 *      Author: mathi
 */

#ifndef HEADERS_MATRIX_H_
#define HEADERS_MATRIX_H_

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


#define COL_1 0x10 // PA4
#define COL_2 0x08 // PA3
#define COL_3 0x04 // PA2

#define ROW_1 0x03 // PE0
#define ROW_2 0x02 // PE1
#define ROW_3 0x01 // PE2
#define ROW_4 0x00 // PE3

#define KEYPAD_QUEUE_LEN 20
#define KEYPAD_QUEUE_ITEM sizeof(keypadStruct)

void init_matrix(void);
void sweep_keypad_task(void *pvParameters);
void keypad_consumer_task(void *pvParameters);

bool keypad_queue_put(keypadStruct queueStruct);
bool keypad_queue_get(keypadStruct *returnStruct);


#endif /* HEADERS_MATRIX_H_ */
