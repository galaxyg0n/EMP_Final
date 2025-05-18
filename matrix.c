/***************** Header *********************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: matrix.c
 * PROJECT: Elevator project
 * DESCRIPTION: Matrix keypad driver for reading and queuing key inputs
 */

/***************** Includes *******************/
#include "matrix.h"

/***************** Variables ******************/
extern QueueHandle_t MATRIX_Q;

/***************** Functions ******************/

void init_matrix(void)
/************************************
*Input   : None
*Output  : None
*Function: Initializes GPIO for keypad matrix input and output
************************************/
{
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;

    volatile unsigned long delay;
    delay = SYSCTL_RCGC2_R;

    GPIO_PORTA_DIR_R |= 0x1C;   // PA2, PA3, PA4 as output (columns)
    GPIO_PORTA_DEN_R |= 0x1C;

    GPIO_PORTE_DEN_R |= 0x0F;   // PE0-PE3 as input (rows)
    GPIO_PORTE_PDR_R |= 0x0F;   // Enable pull-down resistors
}


void sweep_keypad_task(void *pvParameters)
/************************************
*Input   : Task parameters (unused)
*Output  : None
*Function: Scans keypad and sends keypress to MATRIX_Q
************************************/
{
    KEYPAD_STRUCT queue_msg;
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

    char last_key = 0;

    while(1)
    {
        int key_detected = 0;
        int col_index, row_index;

        for(col_index = 0; col_index < 3; col_index++)
        {
            GPIO_PORTA_DATA_R &= 0xE3;            // Clear column bits
            GPIO_PORTA_DATA_R |= col[col_index];  // Set current column

            for(row_index = 0; row_index < 4; row_index++)
            {
                if(GPIO_PORTE_DATA_R & (1 << row[row_index]))
                {
                    char current_key = keymap[row_index][col_index];

                    if(current_key != last_key)
                    {
                        last_key = current_key;
                        queue_msg.keyPressed = current_key;
                        keypad_queue_put(queue_msg);
                    }

                    key_detected = 1;
                }
            }
        }

        if(!key_detected)
            last_key = 0;

        vTaskDelay(10 / portTICK_RATE_MS);
    }
}


bool keypad_queue_put(KEYPAD_STRUCT queueStruct)
/************************************
*Input   : Keypad structure with pressed key
*Output  : True if key was queued successfully, false otherwise
*Function: Sends a keypress to the keypad queue
************************************/
{
    return xQueueSendToBack(MATRIX_Q, &queueStruct, 10 / portTICK_RATE_MS) == pdTRUE;
}


bool keypad_queue_get(KEYPAD_STRUCT *returnStruct)
/************************************
*Input   : Pointer to structure for receiving key
*Output  : True if key was received, false otherwise
*Function: Retrieves a keypress from the keypad queue
************************************/
{
    if(MATRIX_Q != NULL)
    {
        return xQueueReceive(MATRIX_Q, returnStruct, 10 / portTICK_RATE_MS) == pdTRUE;
    }
    return pdFALSE;
}
