/*
 * elevator.h
 *
 *  Created on: 5. maj 2025
 *      Author: karlj
 */

#ifndef HEADERS_ELEVATOR_H_
#define HEADERS_ELEVATOR_H_

#include <stdio.h>
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

void elevator_task(void* pvParameters);

#endif /* HEADERS_ELEVATOR_H_ */
