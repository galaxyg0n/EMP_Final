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

#include "tm4c123gh6pm.h"


#define COL_1 0x04
#define COL_2 0x08
#define COL_3 0x10

#define ROW_1 0x00
#define ROW_2 0x02
#define ROW_3 0x04
#define ROW_4 0x08

#define KEYB_D // PA2
#define KEYB_E // PA3
#define KEYB_F // PA4

#define KEYB_G // PE0
#define KEYB_H // PE1
#define KEYB_J // PE2
#define KEYB_K // PE3


#define KEYPAD_QUEUE_LEN 20
#define KEYPAD_QUEUE_ITEM sizeof(keypadStruct)

void init_matrix(void);
void sweep_keypad_task(void *pvParameters);
void keypad_consumer_task(void *pvParameters);

bool keypad_queue_put(keypadStruct queueStruct);
bool keypad_queue_get(keypadStruct *returnStruct);


#endif /* HEADERS_MATRIX_H_ */
