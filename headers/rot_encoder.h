/*
 * rot_encoder.h
 *
 *  Created on: 12 May 2025
 *      Author: mathi
 */

#ifndef HEADERS_ROT_ENCODER_H_
#define HEADERS_ROT_ENCODER_H_

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

#define DIGI_A  5
#define DIGI_B  6
#define DIGI_P2 7

#define ROTARY_QUEUE_LEN 20
#define ROTARY_QUEUE_ITEM sizeof(ROT_EVENT)

typedef enum {
    SCROLL_UP,
    SCROLL_DOWN,
    BUTTON
} ROT_EVENT;

void rotary_timer_callback(TimerHandle_t xTimer);
void init_rotary();
void rotary_task();


bool rotary_queue_put(char c);


#endif /* HEADERS_ROT_ENCODER_H_ */
