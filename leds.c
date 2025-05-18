/***************** Header *********************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: leds.c
 * PROJECT: Elevator project
 * DESCRIPTION: See module specification in .h file
 */
/***************** Includes *******************/
#include "leds.h"

/***************** Constants ******************/
/***************** Variables ******************/

extern EventGroupHandle_t STATUS_LED_EVENT;
extern double time;

/****************************
 * Function: init_leds
 * -------------------
 * Initializes the GPIO port and pins used for the LEDs (red, yellow, green).
 * Sets pins as output and enables digital function.
 *
 * Input:  None
 * Output: None
 ****************************/
void init_leds()
{
    uint8_t dummy;

    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;  // Enable clock for Port F
    dummy = SYSCTL_RCGC2_R;                 // Dummy read to allow clock to stabilize

    // Set pins connected to LEDs (LED_R, LED_Y, LED_G) as outputs
    GPIO_PORTF_DIR_R |= LED_R + LED_Y + LED_G;

    // Enable digital function for these pins
    GPIO_PORTF_DEN_R |= LED_R + LED_Y + LED_G;

    // Turn off LEDs initially (assuming active low)
    GPIO_PORTF_DATA_R |= LED_R + LED_Y + LED_G;
}

/****************************
 * Function: led_task
 * ------------------
 * FreeRTOS task controlling the status LEDs based on events from STATUS_LED_EVENT.
 * Uses event bits to turn on/off specific LEDs with some timing.
 *
 * Input:  pvParameters (unused)
 * Output: None
 ****************************/
void led_task(void* pvParameters)
{
    const static uint8_t all_leds_bitmask = LED_R | LED_Y | LED_G;
    static EventBits_t eventBits;

    while (1)
    {
        // Wait for any LED event bits with 10 ms timeout
        eventBits = xEventGroupWaitBits(STATUS_LED_EVENT,
                                        LED_R | LED_Y | LED_G,
                                        pdFALSE,   // Don't clear bits on exit
                                        pdFALSE,   // Wait for any bit
                                        10 / portTICK_RATE_MS);

        if (eventBits & CONST_G) // If green LED bit set in event bits
        {
            // Turn on red and yellow LEDs
            GPIO_PORTF_DATA_R |= (all_leds_bitmask) & (LED_R | LED_Y);
            // Turn off green LED
            GPIO_PORTF_DATA_R &= ~LED_G;
        }
        else
        {
            // Turn on LEDs corresponding to bits NOT set
            GPIO_PORTF_DATA_R |= (~eventBits) & all_leds_bitmask;
            // Toggle LEDs corresponding to bits set
            GPIO_PORTF_DATA_R ^= eventBits;
        }

        double delay_time = time / 5;
        vTaskDelay((TickType_t)(delay_time / portTICK_RATE_MS));
    }
}
