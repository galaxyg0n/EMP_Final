/*
 * elevator.c
 *
 *  Created on: 5. maj 2025
 *      Author: karlj
 */

#include "elevator.h"

extern SemaphoreHandle_t E_MOVE_MUTEX;

uint8_t user_floor = 0;
uint8_t dest_floor = 0;


void elevator_task(void* pvParameters)
{
    static uint8_t state = 0;
    static uint8_t current_floor = 2;
    static int8_t direction = 1;

    LCD_Put put1 = {};

    while(1)
    {
        uint8_t* destination = (is_user_in_elevator()) ? &dest_floor : &user_floor;
        switch(state)
        {
        case 0:
            xSemaphoreTake(E_MOVE_MUTEX,portMAX_DELAY);
            direction = (current_floor>*destination) ? 1 : -1;
            break;
        case 1:
            if (current_floor!=*destination)
                current_floor += direction;
            else
            {
                state = 0;
                xSemaphoreGive(E_MOVE_MUTEX);
            }
            vTaskDelay(300/portTICK_RATE_MS);
            break;
        }
    }
}


