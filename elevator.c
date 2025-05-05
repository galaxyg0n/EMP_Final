/*
 * elevator.c
 *
 *  Created on: 5. maj 2025
 *      Author: karlj
 */

#include "elevator.h"
#include "small_sprints.h"

extern SemaphoreHandle_t E_MOVE_MUTEX;

uint8_t user_floor = 5;
uint8_t dest_floor = 0;

uint8_t is_user_in_elevator()
{
    return 0;
}

void elevator_task(void* pvParameters)
{
    static uint8_t state = 0;
    static uint8_t current_floor = 2;
    static int8_t direction = 1;
    static uint8_t str[16];
    const size_t size = sizeof(str);

    while(1)
    {
        uint8_t* destination = (is_user_in_elevator()) ? &dest_floor : &user_floor;
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
            snprintf(str,size,"%d ",current_floor);
            LCD_queue_put(8,1,str);
            if (current_floor!=*destination)
                current_floor += direction;
            else
            {
                state = 0;
                xSemaphoreGive(E_MOVE_MUTEX);
            }
            vTaskDelay(1000/portTICK_RATE_MS);
            break;
        }
    }
}


