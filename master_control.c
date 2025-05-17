/*
 * master_control.c
 *
 *  Created on: 5. maj 2025
 *      Author: karlj
 */

#include "master_control.h"
#include <stdlib.h>



extern QueueHandle_t SW1_E_Q;
extern SemaphoreHandle_t E_MOVE_MUTEX, ROT_ENC_OK, ROT_ENC_FLOOR, ROT_ENC_FIX;
extern EventGroupHandle_t STATUS_LED_EVENT;

extern uint8_t dest_floor;
extern uint16_t pot_val;
volatile uint8_t rot_enc_val;


uint8_t input_key()
{
    keypadStruct returnStruct;
    while(!keypad_queue_get(&returnStruct));
    return returnStruct.keyPressed;
}
void master_control_task(void* pvParameters)
{

    static uint8_t str[32];
    const size_t size = sizeof(str);

    static enum CONT_STATES {E_INIT,E_STILL,E_ARRIVED,E_PASS,E_MOVING,E_BROKEN} cont_state = E_INIT;
    static uint8_t trips;
    xSemaphoreTake(E_MOVE_MUTEX,portMAX_DELAY);
    while(1)
    {
        enum BUTTON_EVENTS be = BE_NO;
        switch(cont_state)
        {
        case E_INIT:
            LCD_queue_put(1,1, "Floor: 2");
            cont_state = E_STILL;
            break;
        case E_STILL:
            xQueueReceive(SW1_E_Q,&be,portMAX_DELAY);
            if (be == BE_LONG)
                cont_state = E_MOVING;
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
            if (is_user_in_elevator(FLIP))
            {
                if (!(++trips%TRIPS_TO_BREAK))
                    cont_state = E_BROKEN;
                else
                    cont_state = E_PASS;
            } else
                cont_state = E_STILL;
            if (trips == 1)
                srand((int)(xTaskGetTickCount()));
            break;

        case E_BROKEN:
        {
            static enum BROKEN_STATES {BROKEN_INIT,BROKEN_MATCH,BROKEN_ENCODER} broken_state = BROKEN_INIT;
            static uint16_t random_val;
            switch(broken_state)
            {
            case BROKEN_INIT:
                LCD_queue_put(1,1,"clc");
                LCD_queue_put(1,1,"Elevator is\nbroken");
                LCD_queue_put(1,1,"clc");
                xEventGroupClearBits(STATUS_LED_EVENT, CONST_G);
                xEventGroupSetBits(STATUS_LED_EVENT,LED_G|LED_R|LED_Y);
                random_val = (rand() % (MAX_POT+1));
                broken_state = BROKEN_MATCH;
                break;

            case BROKEN_MATCH:
            {
                static uint16_t last_dis_val = 0xFFFF;
                if(abs((int)pot_val - (int)last_dis_val) > 3) // Reduces display updates on noise and small movements
                {
                    snprintf(str, size, "Match:%d\n      %d    ", random_val, pot_val);
                    LCD_queue_put(1,1,str);
                    last_dis_val = pot_val;
                }
                if (random_val == pot_val)
                    broken_state = BROKEN_ENCODER;
                break;

            }

            case BROKEN_ENCODER:
                LCD_queue_put(1,1,"clc");
                LCD_queue_put(1,1,"Use encoder:\nDegrees: 0");
                rot_enc_val = 0;
                xSemaphoreGive(ROT_ENC_FIX);
                xSemaphoreTake(ROT_ENC_OK,portMAX_DELAY);
                xEventGroupClearBits(STATUS_LED_EVENT, LED_G|LED_R|LED_Y);
                broken_state = BROKEN_INIT;
                cont_state = E_PASS;
                break;
            }
            vTaskDelay(100 / portTICK_RATE_MS);
            break;
        }
        case E_PASS:
            LCD_queue_put(1,1,"clc");
            LCD_queue_put(1,1,"Password req.\nEnter: ");

            uint8_t num_pos = 7;
            uint16_t password = 0;

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
                LCD_queue_put(1, 1, "Correct!");
                vTaskDelay(500 / portTICK_RATE_MS);
                LCD_queue_put(1, 1, "clc");
                snprintf(str, size, "Destination:\n%d", dest_floor);
                LCD_queue_put(1, 1, str);

                rot_enc_val = dest_floor;
                xSemaphoreGive(ROT_ENC_FLOOR);
                xSemaphoreTake(ROT_ENC_OK,portMAX_DELAY);
                dest_floor = rot_enc_val;
                cont_state = E_MOVING;
            }
            else
                LCD_queue_put(1,1,"Wrong!");

            vTaskDelay(500 / portTICK_RATE_MS);
            break;

        default:
            break;
        }
    }
}



