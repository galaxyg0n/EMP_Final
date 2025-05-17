

/**
 * main.c
 *
 */

#include <LCD.h>
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "glob_def.h"
#include "button.h"
#include "lcd.h"
#include "matrix.h"
#include "uart.h"
#include "elevator.h"
#include "leds.h"
#include "event_groups.h"
#include "rot_encoder.h"
#include "potentiometer.h"

#include "master_control.h"
#include "tm4c123gh6pm.h"

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
QueueHandle_t MATRIX_Q;
QueueHandle_t ROTARY_Q;
QueueHandle_t POT_Q;

TimerHandle_t sw1_timer, rot_debounce;
SemaphoreHandle_t E_MOVE_MUTEX, ROT_ENC_OK, ROT_ENC_FLOOR, ROT_ENC_FIX, ROT_UPD_VAL, ACC_UPD_MUTEX;

EventGroupHandle_t STATUS_LED_EVENT;

void init_hardware()
{
    init_button();
    init_matrix();
    init_leds();
    init_rotary();
    uart_init(9600, 8, NO_PARITY, 1);
    init_adc();

    init_systick();
}

int main(void)
{
    init_hardware();

    sw1_timer = xTimerCreate("Button_timeout", 500 / portTICK_RATE_MS, pdFALSE, NULL, button_timer_callback);    //Button specific
    rot_debounce = xTimerCreate("Rotary encode debounce", 30 / portTICK_RATE_MS, pdFALSE, NULL, rotary_timer_callback);

    SW1_E_Q  = xQueueCreate(8, sizeof(uint8_t));
    LCD_Q    = xQueueCreate(32, sizeof(LCD_Put));
    MATRIX_Q = xQueueCreate(10, sizeof(keypadStruct));
    ROTARY_Q = xQueueCreate(ROTARY_QUEUE_LEN, ROTARY_QUEUE_ITEM);
    POT_Q    = xQueueCreate(10, sizeof(uint16_t));

    E_MOVE_MUTEX  = xSemaphoreCreateMutex();
    ACC_UPD_MUTEX = xSemaphoreCreateMutex();
    ROT_ENC_FLOOR = xSemaphoreCreateBinary();
    ROT_ENC_FIX = xSemaphoreCreateBinary();
    ROT_ENC_OK    = xSemaphoreCreateBinary();

    STATUS_LED_EVENT = xEventGroupCreate();

    xTaskCreate(LCD_task, "LCD", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
    xTaskCreate(sw1_task, "BUTTON", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);

    xTaskCreate(sweep_keypad_task, "SWEEP_KEYPAD", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
    //xTaskCreate(keypad_consumer_task, "KEYPAD", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL); //Can be deleted isnt used anymore
    xTaskCreate(rotary_task, "ROTARY", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);

    xTaskCreate(potentiometer_task, "POTENTIOMETER", LARGE_TASK_STACK_SIZE, NULL, MED_PRIO, NULL);

    xTaskCreate(uart_tx_task, "UART_TX", LARGE_TASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate(uart_rx_task, "UART_RX", LARGE_TASK_STACK_SIZE, NULL, LOW_PRIO, NULL );

    xTaskCreate(master_control_task, "MASTER_CONTROL", USERTASK_STACK_SIZE, NULL, HIGH_PRIO + 1, NULL);
    xTaskCreate(elevator_task, "ELEVATOR", LARGE_TASK_STACK_SIZE, NULL, HIGH_PRIO, NULL);
    xTaskCreate(led_task, "LED", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);

    vTaskStartScheduler();
	return 0;
}
