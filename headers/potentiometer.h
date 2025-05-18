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
#define MAX_POT 4096

/***************** Prototypes *****************/
uint16_t get_adc(void);
void init_adc(void);
void potentiometer_task(void* pvParameters);
void adcvalue_to_string(uint16_t adc_value, char* return_buf, size_t buf_size);
void send_string_uart(const char *str);

#endif /* HEADERS_POTENTIOMETER_H_ */
