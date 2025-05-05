/*
 * master_control.c
 *
 *  Created on: 5. maj 2025
 *      Author: karlj
 */

#include "master_control.h"

#define TRIPS_TO_BREAK 4

extern QueueHandle_t SW1_E_Q, LCD_Q;
extern SemaphoreHandle_t E_MOVE_MUTEX;

void master_control_task(void* pvParameters)
{
    static enum CONT_STATES {E_STILL,E_ARRIVED,E_PASS,E_MOVING,E_BROKEN} cont_state = E_STILL;
    static uint8_t trips = 1;
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
            if (!(trips%TRIPS_TO_BREAK))
                cont_state = E_BROKEN;
            else
                cont_state = E_PASS;
            break;
        case E_BROKEN:
            break;
        default:
            break;
        }
    }
}



