

/**
 * main.c
 *
 */


//Includes
#include "main.h"

//FreeRTOS handles
QueueHandle_t LCD_Q, SW1_E_Q, MATRIX_Q, ROTARY_Q, UART_TX_Q;
TimerHandle_t SW1_TIMER, ROT_DEBOUNCE;
SemaphoreHandle_t E_MOVE_MUTEX, ROT_ENC_OK, ROT_ENC_FLOOR, ROT_ENC_FIX, ROT_UPD_VAL, ACC_UPD_MUTEX;
EventGroupHandle_t STATUS_LED_EVENT;

void init_hardware()
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
{
    //Initialization of hardware peripherals
    init_hardware();

    //Initialize timers
    SW1_TIMER = xTimerCreate("Button_timeout", 500 / portTICK_RATE_MS, pdFALSE, NULL, button_timer_callback);           //Button specific
    ROT_DEBOUNCE = xTimerCreate("Rotary encode debounce", 50 / portTICK_RATE_MS, pdFALSE, NULL, rotary_timer_callback); //Debounce for rotary encoder

    //Initialize queues
    SW1_E_Q  = xQueueCreate(SW1_E_QUEUE_LEN, sizeof(uint8_t));
    LCD_Q    = xQueueCreate(LCD_QUEUE_LEN, sizeof(LCD_PUT));
    MATRIX_Q = xQueueCreate(MATRIX_QUEUE_LEN, sizeof(KEYPAD_STRUCT));
    ROTARY_Q = xQueueCreate(ROTARY_QUEUE_LEN, ROTARY_QUEUE_ITEM);
    UART_TX_Q = xQueueCreate(UART_QUEUE_LEN, sizeof(char)*STR_SIZE);

    //Initialize semaphores
    E_MOVE_MUTEX  = xSemaphoreCreateMutex();
    ACC_UPD_MUTEX = xSemaphoreCreateMutex();
    ROT_ENC_FLOOR = xSemaphoreCreateBinary();
    ROT_ENC_FIX = xSemaphoreCreateBinary();
    ROT_ENC_OK    = xSemaphoreCreateBinary();

    //Initialize event group
    STATUS_LED_EVENT = xEventGroupCreate();

    //Initialize tasks
    xTaskCreate(lcd_task, "LCD", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
    xTaskCreate(sw1_task, "BUTTON", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
    xTaskCreate(sweep_keypad_task, "SWEEP_KEYPAD", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
    xTaskCreate(rotary_task, "ROTARY", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
    xTaskCreate(potentiometer_task, "POTENTIOMETER", LARGE_TASK_STACK_SIZE, NULL, MED_PRIO, NULL);
    xTaskCreate(uart_tx_task, "UART_TX", LARGE_TASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate(uart_rx_task, "UART_RX", LARGE_TASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate(master_control_task, "MASTER_CONTROL", USERTASK_STACK_SIZE, NULL, VERY_HIGH_PRIO, NULL);
    xTaskCreate(elevator_task, "ELEVATOR", LARGE_TASK_STACK_SIZE, NULL, HIGH_PRIO, NULL);
    xTaskCreate(led_task, "LED", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);

    //Run scheduler
    vTaskStartScheduler();

    //Should never reach!
    return 0;
}
