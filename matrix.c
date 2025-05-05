/*
 * matrix.c
 *
 *  Created on: 5 May 2025
 *      Author: mathi
 */

#include "matrix.h"

extern QueueHandle_t MATRIX_Q;

void init_matrix(void)
{
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;

    GPIO_PORTA_DIR_R |= 0x1C;
    GPIO_PORTA_DEN_R |= 0x1C;

    GPIO_PORTE_DEN_R |= 0x0F;
}


void sweep_keypad_task(void *pvParameters)
{
    keypadStruct queue_msg;

    configASSERT(MATRIX_Q);

    uint8_t col[3] = {COL_1, COL_2, COL_3};
    uint8_t row[4] = {ROW_1, ROW_2, ROW_3, ROW_4};

    const char keymap[4][3] =
    {
         {'1', '2', '3'},
         {'4', '5', '6'},
         {'7', '8', '9'},
         {'*', '0', '#'}
    };

    while(1)
    {
        int col_index;
        for(col_index = 0; col_index <= 3; col_index++)
        {
            GPIO_PORTD_DATA_R = col[col_index];

            int row_index;
            for(row_index = 0; row_index <= 4; row_index++)
            {
                if((GPIO_PORTD_DATA_R & (1 << row[row_index])))
                {
                    queue_msg.keyPressed = keymap[row_index][col_index];
                    keypad_queue_put(queue_msg);
                }

            }

        }
    }

    return 0;
}


bool keypad_queue_put(keypadStruct queueStruct)
{
    return xQueueSendToBack(MATRIX_Q, &queueStruct, 10 / portTICK_RATE_MS) == pdTRUE;
}


bool keypad_queue_get(keypadStruct *returnStruct)
{
    if(MATRIX_Q != NULL)
    {
        return xQueueReceive(MATRIX_Q, returnStruct, 10 / portTICK_RATE_MS) == pdTRUE;
    }
    return pdFALSE;
}

void keypad_consumer_task(void *pvParameters)
{
    keypadStruct receivedKey;

    while(1)
    {
        if(keypad_queue_get(&receivedKey))
        {
            uart_queue_put(receivedKey.keyPressed);
        }

        vTaskDelay(10 / portTICK_RATE_MS);
    }
}


