

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
#include "glob_def.h"

//Defines
#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define LARGE_TASK_STACK_SIZE 256
#define VERY_LARGE_TASK_STACK_SIZE 700

#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3

//FreeRTOS handles
QueueHandle_t LCD_Q, SW1_E_Q;
TimerHandle_t sw1_timer;

void init_hardware()
{
    init_systick();
}

int main(void)
{
    sw1_timer = xTimerCreate("Button_timeout",500/portTICK_RATE_MS,pdFALSE,NULL,button_timer_callback);    //Button specific
    SW1_E_Q = xQueueCreate(8, sizeof(uint8_t));

    LCD_Q = xQueueCreate(16,sizeof(LCD_Put));

    xTaskCreate(LCD_task,"LCD",USERTASK_STACK_SIZE,NULL,MED_PRIO,NULL);
    xTaskCreate(sw1_task,"BUTTON",USERTASK_STACK_SIZE,NULL,LOW_PRIO,NULL);

	return 0;
}
