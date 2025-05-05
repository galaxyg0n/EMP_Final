/*
 * button.c
 *
 *  Created on: 3. apr. 2025
 *      Author: karlj
 */
#include "button.h"

extern QueueHandle_t SW1_E_Q;
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
               sw1_state = BS_IDLE
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


