/*
 * button.h
 *
 *  Created on: 3. apr. 2025
 *      Author: karlj
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "systick_frt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "glob_def.h"

//Switch
#define PORTF_LOCK 0x4C4F434B
#define SW1 0x10

void button_timer_callback(TimerHandle_t xTimer);
void sw1_task(void* pvParameters);
void init_button();



#endif /* BUTTON_H_ */
