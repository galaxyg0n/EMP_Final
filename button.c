/***************** Header *********************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: button.c
 * PROJECT: Elevator project
 * DESCRIPTION: Button driver implementation for SW1 input handling and debouncing
 *
 *
 *
 */

/***************** Includes *******************/
#include "button.h"

/***************** External Variables **********/
extern QueueHandle_t SW1_E_Q;
extern TimerHandle_t SW1_TIMER;

/***************** Variables *******************/
uint8_t sw1_timeout = 0;

/***************** Functions ******************/
void button_timer_callback(TimerHandle_t xTimer)
/************************************
 * Input   : xTimer - Timer handle
 * Output  : None
 * Function: Timer callback called when button timeout expires, sets timeout flag
 ************************************/
{
    if (xTimer == SW1_TIMER)
        sw1_timeout = 1;
}

static void set_timer(TimerHandle_t* xTimer)
/************************************
 * Input   : xTimer - pointer to TimerHandle_t variable
 * Output  : None
 * Function: Starts the button press timer and resets timeout flag
 ************************************/
{
    sw1_timeout = 0;
    xTimerStart(*xTimer, 0);
}

static uint8_t is_pressed()
/************************************
 * Input   : None
 * Output  : uint8_t (1 if pressed, 0 if not)
 * Function: Checks if the SW1 button is currently pressed (active low)
 ************************************/
{
    return !(GPIO_PORTF_DATA_R & SW1);
}

void init_button()
/************************************
 * Input   : None
 * Output  : None
 * Function: Initializes GPIO Port F pin for SW1 button input with pull-up and unlocks port
 ************************************/
{
    int dummy;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOF;
    dummy = SYSCTL_RCGCGPIO_R;

    GPIO_PORTF_LOCK_R = PORTF_LOCK;   // Unlock GPIO Port F
    GPIO_PORTF_CR_R |= SW1;            // Allow changes to PF4
    GPIO_PORTF_DIR_R &= ~(SW1);        // Set as input
    GPIO_PORTF_DEN_R |= SW1;            // Digital enable
    GPIO_PORTF_PUR_R |= SW1;            // Internal pull-up resistor
}

void sw1_task(void* pvParameters)
/************************************
 * Input   : pvParameters - FreeRTOS task parameters (unused)
 * Output  : None
 * Function: FreeRTOS task for debouncing SW1 button and detecting button events:
 *           Single press, long press; sends events to queue
 ************************************/
{
    static enum BUTTON_STATES sw1_state = BS_IDLE;

    while (1)
    {
        enum BUTTON_EVENTS be = BE_NO;
        uint8_t button_pressed = is_pressed();

        switch (sw1_state)
        {
            case BS_IDLE:
                if (button_pressed)
                {
                    sw1_state = BS_FP;
                    set_timer(&SW1_TIMER);
                }
                break;

            case BS_FP:
                if (button_pressed)
                {
                    if (sw1_timeout)
                    {
                        sw1_state = BS_LP;
                        be = BE_LONG;
                    }
                }
                else
                {
                    sw1_state = BS_IDLE;
                }
                break;

            case BS_LP:
                if (!button_pressed)
                {
                    sw1_state = BS_IDLE;
                }
                break;

            default:
                break;
        }

        if (be) // If a button event was detected, send it to the event queue
        {
            xQueueSendToBack(SW1_E_Q, &be, portMAX_DELAY);
        }
    }
}
