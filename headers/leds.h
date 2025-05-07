/*
 * leds.h
 *
 *  Created on: 7. maj 2025
 *      Author: karlj
 */

#ifndef HEADERS_LEDS_H_
#define HEADERS_LEDS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "glob_def.h"
#include "event_groups.h"
#include "tm4c123gh6pm.h"

#define LED_R 0x02
#define LED_Y 0x04
#define LED_G 0x08

void init_leds();

void blink_red_led_task(void* pvParameters);
void blink_yellow_led_task(void* pvParameters);
void green_led_task(void* pvParameters);


#endif /* HEADERS_LEDS_H_ */
