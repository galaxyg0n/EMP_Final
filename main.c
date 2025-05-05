

/**
 * main.c
 *
 */

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "lcd.h"

//Defines
#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define LARGE_TASK_STACK_SIZE 256
#define VERY_LARGE_TASK_STACK_SIZE 700

#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3


void init_hardware()
{
    init_systick();
}

int main(void)
{
    xTaskCreate(LCD_task,"LCD",USERTASK_STACK_SIZE,NULL,LOW_PRIO,NULL);

    vTaskStartScheduler();
	return 0;
}
