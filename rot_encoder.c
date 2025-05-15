/*
 * rot_encoder.c
 *
 *  Created on: 12 May 2025
 *      Author: mathi
 */

#include "rot_encoder.h"

extern QueueHandle_t ROTARY_Q;
extern SemaphoreHandle_t ROT_ENC_OK, ROT_ENC_READY;
extern uint8_t rot_enc_val;

void init_rotary()
{
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
    while ((SYSCTL_PRGPIO_R & (1 << 0)) == 0);

    /*
    // Standard setup
    GPIO_PORTA_DEN_R |= 0xE0;
    GPIO_PORTA_PUR_R |= 0xE0;


    GPIO_PORTA_IS_R  &= ~(1 << DIGI_A);                     // Clear mask

    GPIO_PORTA_IEV_R |= ~((1 << DIGI_A) | (1 << DIGI_P2));  // Single falling edge detection

    GPIO_PORTA_ICR_R |= (1 << DIGI_A) | (1 << DIGI_P2);     // Clear interrupts

    GPIO_PORTA_IM_R  |= (1 << DIGI_A) | (1 << DIGI_P2);


    NVIC_EN0_R       |= (1 << 0);
    NVIC_ST_CTRL_R   |= 0x2;
    */
    GPIO_PORTA_DEN_R |= 0xE0;
    GPIO_PORTA_PUR_R |= 0xE0;

    GPIO_PORTA_IS_R  &= ~(1 << DIGI_A);
    GPIO_PORTA_IBE_R  = (1 << DIGI_A);
    GPIO_PORTA_IEV_R |= (1 << DIGI_A) | (1 << DIGI_P2);
    GPIO_PORTA_ICR_R |= (1 << DIGI_A) | (1 << DIGI_P2);
    GPIO_PORTA_IM_R  |= (1 << DIGI_A) | (1 << DIGI_P2);
    NVIC_EN0_R       |= (1 << 0);
    NVIC_ST_CTRL_R   |= 0x2;
}

uint8_t PREV_STATE_A = 0;
uint8_t PREV_STATE_B = 0;
ROT_EVENT rotary_event;

void rotary_ISR_handler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(GPIO_PORTA_MIS_R & (1 << DIGI_P2))
    {
        GPIO_PORTA_ICR_R |= (1 << DIGI_P2);
        rotary_event = BUTTON;
        xQueueSendToBackFromISR(ROTARY_Q, &rotary_event, &xHigherPriorityTaskWoken);
    }

    if(GPIO_PORTA_RIS_R & (1 << DIGI_A))
    {
        GPIO_PORTA_ICR_R |= (1 << DIGI_A);

        uint8_t DIGI_A_STATE = (GPIO_PORTA_DATA_R & (1 << DIGI_A)) >> DIGI_A;
        uint8_t DIGI_B_STATE = (GPIO_PORTA_DATA_R & (1 << DIGI_B)) >> DIGI_B;

        if(DIGI_A_STATE != PREV_STATE_A)
        {
            if((PREV_STATE_A == 0) && (DIGI_A_STATE == 1))
            {
                rotary_event = (DIGI_B_STATE == 1) ? SCROLL_DOWN : SCROLL_UP;
                xQueueSendToBackFromISR(ROTARY_Q, &rotary_event, &xHigherPriorityTaskWoken);

            }
            else if((PREV_STATE_A == 1) && (DIGI_A_STATE == 0))
            {
                rotary_event = (DIGI_B_STATE == 0) ? SCROLL_DOWN : SCROLL_UP;
                xQueueSendToBackFromISR(ROTARY_Q, &rotary_event, &xHigherPriorityTaskWoken);
            }

            PREV_STATE_A = DIGI_A_STATE;
            PREV_STATE_B = DIGI_B_STATE;
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);




}


void rotary_task(void* pvParameters)
{
    configASSERT(ROTARY_Q);
    static char str[32];
    ROT_EVENT event;

    while(1)
    {

        xSemaphoreTake(ROT_ENC_READY, portMAX_DELAY);

        while(event != BUTTON)
        {
            xQueueReceive(ROTARY_Q, &event, portMAX_DELAY);

            if(event == SCROLL_UP || event == SCROLL_DOWN)
            {
                if(event == SCROLL_UP)
                    rot_enc_val = (rot_enc_val < 20) ? rot_enc_val + 1 : 0;
                else
                    rot_enc_val = (rot_enc_val > 0) ? rot_enc_val - 1 : 20;

                if(rot_enc_val == 13)
                    rot_enc_val += (event == SCROLL_UP) ? 1 : -1;

                snprintf(str, sizeof(str), "%d ", rot_enc_val);
                LCD_queue_put(1, 2, str);
            }

            GPIO_PORTA_IM_R  |= (1 << DIGI_A) | (1 << DIGI_P2);     // Unmasking
            vTaskDelay(200 / portTICK_RATE_MS);
        }

        xSemaphoreGive(ROT_ENC_OK);
    }
}
