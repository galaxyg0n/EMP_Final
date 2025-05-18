/***************** Source File ****************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: rot_encoder.c
 * PROJECT: Elevator project
 * DESCRIPTION: Rotary encoder driver and task implementation
 *
 * Created on: [insert date]
 * Author: karlj
 */

#include "rot_encoder.h"

/***************** External Variables ****************/
extern QueueHandle_t ROTARY_Q;
extern SemaphoreHandle_t ROT_ENC_OK, ROT_ENC_FLOOR, ROT_ENC_FIX;
extern uint8_t rot_enc_val;
extern TimerHandle_t ROT_DEBOUNCE;

/***************** Local Defines ********************/
#define FULL_TURN 30

/***************** Local Variables *******************/
static uint8_t PREV_STATE_A = 0;
static uint8_t PREV_STATE_B = 0;
static ROT_EVENT rotary_event;

/***************** Function Prototypes ***************/
static void set_timer(TimerHandle_t* xTimer);

/***************** Functions ******************/

void init_rotary()
/************************************
 * Input   : None
 * Output  : None
 * Function: Initialize rotary encoder GPIO pins, interrupts and timer
 ************************************/
{
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
    while ((SYSCTL_PRGPIO_R & (1 << 0)) == 0);

    GPIO_PORTA_DEN_R |= 0xE0;         // Enable digital on PA5-7
    GPIO_PORTA_PUR_R |= 0xE0;         // Enable pull-up resistors on PA5-7

    GPIO_PORTA_IS_R  &= ~(1 << DIGI_A);  // Edge-sensitive interrupts on DIGI_A
    GPIO_PORTA_IBE_R  = (1 << DIGI_A);    // Both edges interrupt on DIGI_A
    GPIO_PORTA_IEV_R |= (1 << DIGI_A) | (1 << DIGI_P2); // Rising edge for DIGI_A and DIGI_P2
    GPIO_PORTA_ICR_R |= (1 << DIGI_A) | (1 << DIGI_P2); // Clear any prior interrupts
    GPIO_PORTA_IM_R  |= (1 << DIGI_A) | (1 << DIGI_P2); // Unmask interrupts for DIGI_A and DIGI_P2

    NVIC_EN0_R       |= (1 << 0);          // Enable interrupt 0 in NVIC
    NVIC_ST_CTRL_R   |= 0x2;                // Enable SysTick interrupt (if needed)
}

void rotary_timer_callback(TimerHandle_t xTimer)
/************************************
 * Input   : xTimer - Timer handle
 * Output  : None
 * Function: Timer callback to unmask rotary encoder GPIO interrupts after debounce
 ************************************/
{
    if (xTimer == ROT_DEBOUNCE)
        GPIO_PORTA_IM_R  |= (1 << DIGI_A) | (1 << DIGI_P2);     // Unmask interrupts
}

static void set_timer(TimerHandle_t* xTimer)
/************************************
 * Input   : xTimer - Timer handle pointer
 * Output  : None
 * Function: Mask rotary encoder GPIO interrupts and start debounce timer
 ************************************/
{
    GPIO_PORTA_IM_R  &= ~((1 << DIGI_A) | (1 << DIGI_P2));     // Mask interrupts
    xTimerStart(*xTimer, 0);
}

void rotary_ISR_handler(void)
/************************************
 * Input   : None
 * Output  : None
 * Function: GPIO Port A interrupt handler for rotary encoder signals and button press
 ************************************/
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(GPIO_PORTA_MIS_R & (1 << DIGI_P2))
    {
        GPIO_PORTA_ICR_R |= (1 << DIGI_P2);    // Clear interrupt flag
        rotary_event = BUTTON;
        xQueueSendToBackFromISR(ROTARY_Q, &rotary_event, &xHigherPriorityTaskWoken);
    }

    if(GPIO_PORTA_MIS_R & (1 << DIGI_A))
    {
        GPIO_PORTA_ICR_R |= (1 << DIGI_A);     // Clear interrupt flag

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

    set_timer(&ROT_DEBOUNCE);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void rotary_task(void* pvParameters)
/************************************
 * Input   : pvParameters - FreeRTOS task parameters
 * Output  : None
 * Function: Main task to handle rotary encoder events and update encoder value and display
 ************************************/
{
    configASSERT(ROTARY_Q);
    static ROT_EVENT fix_turn_direction = SCROLL_DOWN;
    char str[STR_SIZE];

    while(1)
    {
        if (xSemaphoreTake(ROT_ENC_FLOOR, 0) == pdTRUE)
        {
            xQueueReset(ROTARY_Q);
            ROT_EVENT event = SCROLL_DOWN;
            while(event != BUTTON)
            {
                xQueueReceive(ROTARY_Q, &event, portMAX_DELAY);
                if(event == SCROLL_UP || event == SCROLL_DOWN)
                {
                    if(event == SCROLL_UP)
                        rot_enc_val = (rot_enc_val < 20) ? rot_enc_val + 1 : 0;
                    else
                        rot_enc_val = (rot_enc_val > 0) ? rot_enc_val - 1 : 20;

                    if(rot_enc_val == FORBIDDEN_FLOOR)
                        rot_enc_val += (event == SCROLL_UP) ? 1 : -1;
                }
                snprintf(str, sizeof(str), "%d ", rot_enc_val);
                lcd_queue_put(1, 2, str);
            }
            xSemaphoreGive(ROT_ENC_OK);
        }

        if (xSemaphoreTake(ROT_ENC_FIX, 0) == pdTRUE)
        {
            xQueueReset(ROTARY_Q);
            ROT_EVENT event = SCROLL_UP;
            fix_turn_direction = (fix_turn_direction == SCROLL_UP) ? SCROLL_DOWN : SCROLL_UP;

            while(event != BUTTON)
            {
                xQueueReceive(ROTARY_Q, &event, portMAX_DELAY);
                if (event == fix_turn_direction)
                {
                    if(++rot_enc_val > FULL_TURN)
                        rot_enc_val = 0;

                    snprintf(str, sizeof(str), " %d     ", (360 / FULL_TURN) * rot_enc_val);
                    lcd_queue_put(9, 2, str);
                }
                else
                {
                    lcd_queue_put(9, 2, "DIR_ERR");
                }
            }

            if (rot_enc_val == FULL_TURN)
            {
                xSemaphoreGive(ROT_ENC_OK);
            }
            else
            {
                lcd_queue_put(9, 2, "DEG_ERR");
                xSemaphoreGive(ROT_ENC_FIX);
            }
        }

        vTaskDelay(50 / portTICK_RATE_MS);
    }
}
