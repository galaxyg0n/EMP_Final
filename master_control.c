/*
 * master_control.c
 *
 *  Created on: 5. maj 2025
 *      Author: karlj
 */

#include "master_control.h"

#define TRIPS_TO_BREAK 4

extern QueueHandle_t SW1_E_Q;
extern SemaphoreHandle_t E_MOVE_MUTEX;
extern EventGroupHandle_t STATUS_LED_EVENT;

extern uint8_t dest_floor;


uint8_t input_key()
{
    keypadStruct returnStruct;
    while(!keypad_queue_get(&returnStruct));
    return returnStruct.keyPressed;
}

void master_control_task(void* pvParameters)
{

    static uint8_t str[16];
    const size_t size = sizeof(str);

    static enum CONT_STATES {E_STILL,E_ARRIVED,E_PASS,E_MOVING,E_BROKEN} cont_state = E_STILL;
    static uint8_t trips = 1;
    xSemaphoreTake(E_MOVE_MUTEX,portMAX_DELAY);
    while(1)
    {
        enum BUTTON_EVENTS be = BE_NO;
        switch(cont_state)
        {
        case E_STILL:
            xQueueReceive(SW1_E_Q,&be,portMAX_DELAY);
            if (be == BE_LONG)
            {
                trips++;
                cont_state = E_MOVING;
            }
            break;

        case E_MOVING:
            xSemaphoreGive(E_MOVE_MUTEX);
            vTaskDelay(100/portTICK_RATE_MS);
            xSemaphoreTake(E_MOVE_MUTEX,portMAX_DELAY);
            cont_state = E_ARRIVED;
            break;

        case E_ARRIVED:
            LCD_queue_put(1,2,"Arrived!       ");
            vTaskDelay(750/portTICK_RATE_MS);
            //Check user in elevator state and flip it
            if (is_user_in_elevator(1))
            {
                if (!(trips%TRIPS_TO_BREAK))
                    cont_state = E_BROKEN;
                else
                    cont_state = E_PASS;
            } else
                cont_state = E_STILL;
            break;

        case E_BROKEN:
            xEventGroupClearBits(STATUS_LED_EVENT, CONST_G);
            xEventGroupSetBits(STATUS_LED_EVENT,LED_G|LED_R|LED_Y);
            while(1);
            break;

        case E_PASS:
            LCD_queue_put(1,1,"clc");
            LCD_queue_put(1,1,"Password req.\nEnter: ");

            uint8_t num_pos = 7;
            uint8_t password = 0;

            uint16_t pass_pos;
            for (pass_pos = 1000; pass_pos; pass_pos/=10)
            {
                uint8_t entered_val = input_key();
                snprintf(str,size,"%c",entered_val);
                password += (entered_val-'0')*pass_pos;
                LCD_queue_put(num_pos++,2,str);
            }

            LCD_queue_put(1,1,"clc");
            if (!(password%8))
            {
                LCD_queue_put(1,1,"Correct!");
                dest_floor = 14;
                cont_state = E_MOVING;
                //Choose floor with rotary encoder
                //Call state E_MOVING again
            }
            else
                LCD_queue_put(1,1,"Wrong!");

            vTaskDelay(500/portTICK_RATE_MS);
            break;

        default:
            break;
        }
    }
}



