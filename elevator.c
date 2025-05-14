/*
 * elevator.c
 *
 *  Created on: 5. maj 2025
 *      Author: karlj
 */

#include "elevator.h"
#include "small_sprints.h"

#define FORBIDDEN_FLOOR 13

extern SemaphoreHandle_t E_MOVE_MUTEX;
extern EventGroupHandle_t STATUS_LED_EVENT;

volatile uint8_t dest_floor = 0;

uint8_t is_user_in_elevator(uint8_t choice)
{
    static uint8_t user_in_elevator = 0;
    user_in_elevator ^= choice;
    return user_in_elevator;
}

void elevator_task(void* pvParameters)
{
    static enum ELE_STATES {NO_MOTION, INIT_MOTION, IN_MOTION} ele_state = NO_MOTION;

    static int8_t user_floor = 0;
    static int8_t current_floor = 2;

    static uint8_t str[32];
    const size_t size = sizeof(str);
    while(1)
    {

        switch(ele_state)
        {
        case NO_MOTION:
            xSemaphoreTake(E_MOVE_MUTEX,portMAX_DELAY);
            uint8_t* destination = (is_user_in_elevator(NO_FLIP)) ? &dest_floor : &user_floor;
            int8_t direction = (current_floor>*destination) ? -1 : 1;
            ele_state = 1;
            break;
        case INIT_MOTION:
            LCD_queue_put(1,1,"clc");
            snprintf(str,size,"Floor: %d\n%s",current_floor,(direction+1)?"Going up!" : "Going down!");
            LCD_queue_put(1,1,str);
            ele_state = IN_MOTION;
            break;
        case IN_MOTION:
        {
            uint8_t extra_floor = ((*destination>FORBIDDEN_FLOOR&&current_floor<FORBIDDEN_FLOOR)||(*destination<FORBIDDEN_FLOOR&&current_floor>FORBIDDEN_FLOOR));
            uint8_t floor_50_pct = (*destination+current_floor-extra_floor)/2;

            xEventGroupClearBits(STATUS_LED_EVENT,CONST_G|LED_R);
            xEventGroupSetBits(STATUS_LED_EVENT,LED_Y);

            while(current_floor!=*destination)
            {
                vTaskDelay(1000/portTICK_RATE_MS);
                if ((current_floor+=direction) == floor_50_pct)
                {
                    xEventGroupClearBits(STATUS_LED_EVENT,LED_Y);
                    xEventGroupSetBits(STATUS_LED_EVENT,LED_R);
                }
                if (current_floor == FORBIDDEN_FLOOR)
                    current_floor += direction;

                snprintf(str,size,"%d ",current_floor);
                LCD_queue_put(8,1,str);
            }

            user_floor = current_floor;
            xEventGroupClearBits(STATUS_LED_EVENT,LED_R|LED_Y);
            xEventGroupSetBits(STATUS_LED_EVENT,CONST_G);
            ele_state = NO_MOTION;

            xSemaphoreGive(E_MOVE_MUTEX);
            vTaskDelay(100/portTICK_RATE_MS);

            break;
        }
        }
    }
}


