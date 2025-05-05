/*
 * button.c
 *
 *  Created on: 3. apr. 2025
 *      Author: karlj
 */
#include "button.h"
#include "lcd.h"

extern QueueHandle_t SW1_E_Q, LCD_Q;
extern TimerHandle_t sw1_timer;
uint8_t sw1_timeout;

//Function: Sets timeout event when timer runs out
void button_timer_callback(TimerHandle_t xTimer)
{
    if (xTimer == sw1_timer)
        sw1_timeout = 1;
}

//Function: Starts timer
static void set_timer(TimerHandle_t* xTimer)
{
    sw1_timeout = 0;
    xTimerStart(*xTimer,0);
}

static uint8_t is_pressed()
{
    return !(GPIO_PORTF_DATA_R & SW1);
}

void init_button()
{
    int dummy;
    SYSCTL_RCGCGPIO_R  |= SYSCTL_RCGC2_GPIOF;
    dummy = SYSCTL_RCGCGPIO_R;

    GPIO_PORTF_LOCK_R = PORTF_LOCK;   //Unlock GPIO Port F
    GPIO_PORTF_CR_R |= SW1;           //Allow changes to PF4
    GPIO_PORTF_DIR_R &= ~(SW1);       //Set as input
    GPIO_PORTF_DEN_R |= SW1;          //Digital enable
    GPIO_PORTF_PUR_R |= SW1;          //Internal pull-up resistor
}

/*---------------------------
 * Task: Takes button inputs and return the appropriate event
 * Output: Either single press, double press or long press
 --------------------------*/
void sw1_task(void* pvParameters)
{
    static enum BUTTON_STATES sw1_state = BS_IDLE;
    while(1)
    {
       enum BUTTON_EVENTS be = BE_NO;
       uint8_t button_pressed = is_pressed();

       switch(sw1_state)
       {
       case BS_IDLE:
           if(button_pressed)
           {
               sw1_state = BS_FP;
               set_timer(&sw1_timer);
           }
           break;
       case BS_FP:
           if(button_pressed)
           {
               if(sw1_timeout)
               {
                   sw1_state = BS_LP;
                   be = BE_LONG;
               }
           }
           else
               sw1_state = BS_IDLE;
           break;
       case BS_LP:
           if (!button_pressed)
               sw1_state = BS_IDLE;
           break;
       default:
           break;
       }

       if (be) //If at button event is set, sends that to the button event queue
           xQueueSendToBack(SW1_E_Q,&be,portMAX_DELAY);
    }
}


