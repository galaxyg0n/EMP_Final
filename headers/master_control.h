/*
 * master_control.h
 *
 *  Created on: 5. maj 2025
 *      Author: karlj
 */

#ifndef HEADERS_MASTER_CONTROL_H_
#define HEADERS_MASTER_CONTROL_H_

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"
#include "lcd.h"
#include "leds.h"
#include "glob_def.h"
#include "elevator.h"

void master_control_task(void* pvParameters);


#endif /* HEADERS_MASTER_CONTROL_H_ */
