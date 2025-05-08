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
    static uint8_t state = 0;
    static uint8_t user_floor = 0;
    static uint8_t current_floor = 2;
    static int8_t direction = 1;
    static uint8_t str[16];
    static float perc_trip;
    const size_t size = sizeof(str);

    while(1)
    {
        uint8_t* destination = (is_user_in_elevator(0)) ? &dest_floor : &user_floor;
        switch(state)
        {
        case 0:
            xSemaphoreTake(E_MOVE_MUTEX,portMAX_DELAY);
            direction = (current_floor>*destination) ? -1 : 1;
            state = 1;
            break;
        case 1:
            LCD_queue_put(1,1,"clc");
            snprintf(str,size,"Floor: %d",current_floor);
            LCD_queue_put(1,1,str);
            snprintf(str,size,"%s",(direction+1)?"Going up!" : "Going down!");
            LCD_queue_put(1,2,str);
            state = 2;
            break;
        case 2:
        {
            float extra_floor = (float)((*destination>FORBIDDEN_FLOOR&&current_floor<FORBIDDEN_FLOOR)||(*destination<FORBIDDEN_FLOOR&&current_floor>FORBIDDEN_FLOOR));
            float floor_pct_inc = 100/((float)(*destination)-(float)current_floor-extra_floor);
            float floor_pct = 0;

            while(current_floor!=*destination)
            {
                vTaskDelay(1000/portTICK_RATE_MS);
                current_floor+=direction;
                if (current_floor == FORBIDDEN_FLOOR)
                    current_floor += direction;
                snprintf(str,size,"%d ",current_floor);
                LCD_queue_put(8,1,str);


                floor_pct+=floor_pct_inc;
                xEventGroupClearBits(STATUS_LED_EVENT,LED_R|LED_Y|LED_G);
                if (floor_pct<50)
                    xEventGroupSetBits(STATUS_LED_EVENT,LED_Y);
                else if (floor_pct<100)
                    xEventGroupSetBits(STATUS_LED_EVENT,LED_R);
            }
            user_floor = current_floor;
            xEventGroupSetBits(STATUS_LED_EVENT,LED_G);
            state = 0;
            xSemaphoreGive(E_MOVE_MUTEX);
            vTaskDelay(100/portTICK_RATE_MS);

            break;
        }
        }
    }
}


