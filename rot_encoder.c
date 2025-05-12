/*
 * rot_encoder.c
 *
 *  Created on: 12 May 2025
 *      Author: mathi
 */

#include "rot_encoder.h"

extern QueueHandle_t ROTARY_Q;

void init_rotary()
{
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
    while ((SYSCTL_PRGPIO_R & (1 << 0)) == 0);

    // Standard setup
    GPIO_PORTA_DIR_R |= ~0xE0;
    GPIO_PORTA_DEN_R |= 0xE0;
    GPIO_PORTA_PUR_R |= 0xE0;

    GPIO_PORTA_IS_R  &= ~(1 << DIGI_A);                     // Clear mask

    GPIO_PORTA_IBE_R &= ~((1 << DIGI_A) | (1 << DIGI_P2));  // IEV control
    GPIO_PORTA_IEV_R |= ~((1 << DIGI_A) | (1 << DIGI_P2));  // Single falling edge detection

    GPIO_PORTA_ICR_R |= (1 << DIGI_A) | (1 << DIGI_P2);     // Clear interrupts
    GPIO_PORTA_IM_R  |= (1 << DIGI_A) | (1 << DIGI_P2);     // Unmasking

    NVIC_EN0_R       |= (1 << 0);
    NVIC_ST_CTRL_R   |= 0x2;
}


void rotary_ISR_handler(void)
{
    if(GPIO_PORTA_RIS_R & (1 << DIGI_A))
    {
        GPIO_PORTA_ICR_R |= (1 << DIGI_A);
        rotary_queue_put('1');
    }

    if(GPIO_PORTA_MIS_R & (1 << DIGI_P2))
    {
        GPIO_PORTA_ICR_R |= (1 << DIGI_P2);
        rotary_queue_put('2');
    }
}


void rotary_task(void* pvParameters)
{
    configASSERT(ROTARY_Q);
    char c;

    while(1)
    {
        if(xQueueReceive(ROTARY_Q, &c, portMAX_DELAY) == pdPASS)
        {
            uart_queue_put(c);
        }
    }
}


bool rotary_queue_put(char c)
{
    return xQueueSendToBack(ROTARY_Q, &c, 10 / portTICK_RATE_MS) == pdTRUE;
}
