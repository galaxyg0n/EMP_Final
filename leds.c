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

void led_task(void* pvParameters)
{
    const static all_leds_bitmask = LED_R|LED_Y|LED_G;
    static EventBits_t eventBits;
    while(1)
    {
        eventBits = xEventGroupWaitBits(STATUS_LED_EVENT,LED_R|LED_Y|LED_G,pdFALSE,pdFALSE,10/portTICK_RATE_MS);
        if (eventBits & CONST_G)
        {
            GPIO_PORTF_DATA_R |= (all_leds_bitmask)&(LED_R|LED_Y);
            GPIO_PORTF_DATA_R &= ~(LED_G);
        }
        else
        {
            GPIO_PORTF_DATA_R |= (~eventBits)&all_leds_bitmask;
            GPIO_PORTF_DATA_R ^= eventBits;
        }

        vTaskDelay(200/portTICK_RATE_MS);
    }
}

