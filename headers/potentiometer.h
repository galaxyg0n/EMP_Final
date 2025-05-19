/***************** Header File ****************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: potentiometer.h
 * PROJECT: Elevator project
 * DESCRIPTION: Header file for potentiometer ADC interface and utilities
 *
 *
 * 
 */

#ifndef HEADERS_POTENTIOMETER_H_
#define HEADERS_POTENTIOMETER_H_

/***************** Includes *******************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "glob_def.h"

/***************** Defines ********************/
#define MAX_POT 4095
#define END0 0x02
#define SEQ3 0x08
#define SEQ_PRI 0x00000123
#define EM3 12
#define ADC_INPUT_CHANNEL 0x0B

/***************** Prototypes *****************/
uint16_t get_adc(void);
void init_adc(void);
void potentiometer_task(void* pvParameters);


#endif /* HEADERS_POTENTIOMETER_H_ */
