/*
 * elevator.c
 *
 *  Created on: 5. maj 2025
 *      Author: karlj
 */

#include "elevator.h"

extern SemaphoreHandle_t E_MOVE_MUTEX, ACC_UPD_MUTEX;
extern EventGroupHandle_t STATUS_LED_EVENT;

volatile uint8_t dest_floor = 0;
volatile double acceleration = 1;
double time = 1000;

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

    static char log_msg[STR_SIZE];
    static char str[STR_SIZE];

    while(1)
    {
        switch(ele_state)
        {
        case NO_MOTION:
            xSemaphoreTake(E_MOVE_MUTEX,portMAX_DELAY);
            volatile uint8_t* destination = (is_user_in_elevator(NO_FLIP)) ? &dest_floor : &user_floor;
            int8_t direction = (current_floor>*destination) ? -1 : 1;
            ele_state = 1;
            break;
        case INIT_MOTION:
            lcd_queue_put(1,1,"clc");
            snprintf(str,sizeof(str),"Floor: %d\n%s",current_floor,(direction+1)?"Going up!" : "Going down!");
            lcd_queue_put(1,1,str);
            ele_state = IN_MOTION;
            break;
        case IN_MOTION:
        {
            uint8_t extra_floor = ((*destination>FORBIDDEN_FLOOR&&current_floor<FORBIDDEN_FLOOR)||(*destination<FORBIDDEN_FLOOR&&current_floor>FORBIDDEN_FLOOR));
            uint8_t floor_50_pct = (*destination+current_floor-extra_floor)/2;

            xEventGroupClearBits(STATUS_LED_EVENT,CONST_G|LED_R);
            xEventGroupSetBits(STATUS_LED_EVENT,LED_Y);

            uint8_t og_floor = current_floor;

            xSemaphoreTake(ACC_UPD_MUTEX,portMAX_DELAY);
            double v_0 = 0;
            double tot_time = 0;
            while(current_floor!=*destination)
            {
                time = ((sqrt(v_0*v_0+2*acceleration))-v_0)/acceleration;
                if (time<0)
                    time = sqrt(2);
                v_0 = v_0+acceleration*time;
                tot_time += time;
                time *= 1000;

                vTaskDelay(time/portTICK_RATE_MS);
                if ((current_floor+=direction) == floor_50_pct)
                {
                    acceleration*=-1;
                    xEventGroupClearBits(STATUS_LED_EVENT,LED_Y);
                    xEventGroupSetBits(STATUS_LED_EVENT,LED_R);
                }
                if (current_floor == FORBIDDEN_FLOOR)
                    current_floor += direction;

                snprintf(str,sizeof(str),"%d ",current_floor);
                lcd_queue_put(8,1,str);
            }
            if (acceleration<0)
                acceleration *= -1;
            time = 1000;

            xSemaphoreGive(ACC_UPD_MUTEX);

            snprintf(log_msg,sizeof(log_msg),"FROM:%d,TO:%d,IN:%ds,USER:%s\n",og_floor,current_floor,(uint8_t)tot_time,is_user_in_elevator(NO_FLIP)?"OUT":"IN");
            uart_queue_put(log_msg);

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


