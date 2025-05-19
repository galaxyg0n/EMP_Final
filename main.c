/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: main.c
*
* PROJECT....: Elevator
*
* DESCRIPTION: System main module with FreeRTOS setup, initialization of
*              hardware, tasks, timers, queues, semaphores, and events.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "main.h"

/*****************************   Variables   *******************************/

// FreeRTOS handles
QueueHandle_t LCD_Q, SW1_E_Q, MATRIX_Q, ROTARY_Q, UART_TX_Q;
TimerHandle_t SW1_TIMER, ROT_DEBOUNCE;
SemaphoreHandle_t E_MOVE_MUTEX, ROT_ENC_OK, ROT_ENC_FLOOR, ROT_ENC_FIX, ROT_UPD_VAL, ACC_UPD_MUTEX;
EventGroupHandle_t STATUS_LED_EVENT;

/*****************************   Functions   *******************************/

void init_hardware()
/*****************************************************************************
*   Function : Initializes all system hardware peripherals.
******************************************************************************/
{
    init_button();
    init_matrix();
    init_leds();
    init_rotary();
    init_uart(9600, 8, NO_PARITY, 1);
    init_adc();
    init_systick();
}

int main(void)
/*****************************************************************************
*   Function : Main entry point of the system. Initializes hardware,
*              creates RTOS tasks, queues, timers, semaphores, and starts
*              the FreeRTOS scheduler.
******************************************************************************/
{
    // Initialization of hardware peripherals
    init_hardware();

    // Create software timers
    SW1_TIMER = xTimerCreate("Button_timeout", 500 / portTICK_RATE_MS, pdFALSE, NULL, button_timer_callback);           // Button debounce timeout
    ROT_DEBOUNCE = xTimerCreate("Rotary encode debounce", 50 / portTICK_RATE_MS, pdFALSE, NULL, rotary_timer_callback); // Rotary encoder debounce

    // Create queues
    SW1_E_Q   = xQueueCreate(SW1_E_QUEUE_LEN, sizeof(uint8_t));                 // Button event queue
    LCD_Q     = xQueueCreate(LCD_QUEUE_LEN, sizeof(LCD_PUT));                  // LCD command queue
    MATRIX_Q  = xQueueCreate(MATRIX_QUEUE_LEN, sizeof(KEYPAD_STRUCT));         // Keypad matrix queue
    ROTARY_Q  = xQueueCreate(ROTARY_QUEUE_LEN, ROTARY_QUEUE_ITEM);             // Rotary encoder queue
    UART_TX_Q = xQueueCreate(UART_QUEUE_LEN, sizeof(char) * STR_SIZE);         // UART TX queue

    // Create semaphores
    E_MOVE_MUTEX  = xSemaphoreCreateMutex();       // Elevator movement control mutex
    ACC_UPD_MUTEX = xSemaphoreCreateMutex();       // Accelerometer data update mutex
    ROT_ENC_FLOOR = xSemaphoreCreateBinary();      // Floor selection from rotary encoder
    ROT_ENC_FIX   = xSemaphoreCreateBinary();      // Rotary fix semaphore
    ROT_ENC_OK    = xSemaphoreCreateBinary();      // Rotary OK press semaphore

    // Create event group
    STATUS_LED_EVENT = xEventGroupCreate();        // Status LED synchronization

    // Create FreeRTOS tasks
    xTaskCreate(lcd_task,              "LCD",             USERTASK_STACK_SIZE,       NULL, MED_PRIO, NULL);
    xTaskCreate(sw1_task,              "BUTTON",          USERTASK_STACK_SIZE,       NULL, LOW_PRIO, NULL);
    xTaskCreate(sweep_keypad_task,     "SWEEP_KEYPAD",    USERTASK_STACK_SIZE,       NULL, MED_PRIO, NULL);
    xTaskCreate(rotary_task,           "ROTARY",          USERTASK_STACK_SIZE,       NULL, MED_PRIO, NULL);
    xTaskCreate(potentiometer_task,    "POTENTIOMETER",   LARGE_TASK_STACK_SIZE,     NULL, MED_PRIO, NULL);
    xTaskCreate(uart_tx_task,          "UART_TX",         LARGE_TASK_STACK_SIZE,     NULL, LOW_PRIO, NULL);
    xTaskCreate(uart_rx_task,          "UART_RX",         LARGE_TASK_STACK_SIZE,     NULL, LOW_PRIO, NULL);
    xTaskCreate(master_control_task,   "MASTER_CONTROL",  USERTASK_STACK_SIZE,       NULL, VERY_HIGH_PRIO, NULL);
    xTaskCreate(elevator_task,         "ELEVATOR",        LARGE_TASK_STACK_SIZE,     NULL, HIGH_PRIO, NULL);
    xTaskCreate(led_task,              "LED",             USERTASK_STACK_SIZE,       NULL, LOW_PRIO, NULL);

    // Start FreeRTOS scheduler
    vTaskStartScheduler();

    // Should never reach here
    return 0;
}

/****************************** End Of Module *******************************/
