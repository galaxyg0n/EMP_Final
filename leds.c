/*
 * leds.c
 *
 *  Created on: 7. maj 2025
 *      Author: karlj
 */
#include "leds.h"

extern EventGroupHandle_t STATUS_LED_EVENT;

void init_leds()
{
      uint8_t dummy;
      SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
      dummy = SYSCTL_RCGC2_R;

      GPIO_PORTF_DIR_R |= LED_R+LED_Y+LED_G;
      GPIO_PORTF_DEN_R |= LED_R+LED_Y+LED_G;
      GPIO_PORTF_DATA_R |= LED_R+LED_Y+LED_G;
}

void blink_red_led_task(void* pvParameters)
{
    while(1)
    {
        if (xEventGroupWaitBits(STATUS_LED_EVENT,LED_R,pdFALSE,pdFALSE,10/portTICK_RATE_MS) & LED_R)
            GPIO_PORTF_DATA_R ^= LED_R;
        else
            GPIO_PORTF_DATA_R |= LED_R;
        vTaskDelay(200/portTICK_RATE_MS);
    }
}

void blink_yellow_led_task(void* pvParameters)
{
    while(1)
    {
        if (xEventGroupWaitBits(STATUS_LED_EVENT,LED_Y,pdFALSE,pdFALSE,10/portTICK_RATE_MS) & LED_Y)
            GPIO_PORTF_DATA_R ^= LED_Y;
        else
            GPIO_PORTF_DATA_R |= LED_Y;
        vTaskDelay(200/portTICK_RATE_MS);
    }
}

void green_led_task(void* pvParameters)
{
    while(1)
    {
        if (xEventGroupWaitBits(STATUS_LED_EVENT,LED_G,pdFALSE,pdFALSE,10/portTICK_RATE_MS) & LED_G)
            GPIO_PORTF_DATA_R &= ~(LED_G);
        else
            GPIO_PORTF_DATA_R |= LED_G;
        vTaskDelay(200/portTICK_RATE_MS);
    }
}

