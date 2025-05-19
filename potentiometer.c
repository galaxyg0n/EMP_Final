/***************** Source File ****************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: potentiometer.c
 * PROJECT: Elevator project
 * DESCRIPTION: Source file for potentiometer ADC reading and filtering
 *
 * 
 */

#include "potentiometer.h"

/***************** Global Variables ***********/
uint16_t pot_val;

/***************** Functions ******************/

/************************************
* Input   : None
* Output  : ADC result (0-4095)
* Function: Returns the latest ADC conversion result from FIFO3
************************************/
uint16_t get_adc()
{
    return ADC0_SSFIFO3_R;
}

/************************************
* Input   : None
* Output  : None
* Function: Initializes ADC0 and GPIOB for analog input
************************************/
void init_adc()
{
    SYSCTL_RCGC0_R |= SYSCTL_RCGC0_ADC0;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;

    // Set ADC0 Sequencer priorities
    ADC0_SSPRI_R = 0x00000123;

    // Disable all sequencers
    ADC0_ACTSS_R = 0;

    // Set sequencer 3 trigger to always
    ADC0_EMUX_R = 0x0000F000;

    // Select ADC input channel 11 for sequencer 3
    ADC0_SSMUX3_R = 0x0B;

    // Configure sequence control (END0)
    ADC0_SSCTL3_R = 0x00000002;

    // Enable sequencer 3
    ADC0_ACTSS_R = 0x00000008;

    // Start conversion on sequencer 3
    ADC0_PSSI_R = 0x08;
}

/************************************
* Input   : pvParameters (unused)
* Output  : None
* Function: FreeRTOS task that filters ADC values using a moving average
************************************/
void potentiometer_task(void* pvParameters)
{
    uint16_t former_xvals[] = {0, 0, 0};
    const uint8_t length = (uint8_t)(sizeof(former_xvals) / sizeof(uint16_t));

    while(1)
    {
        // Makes a short simple moving average to try and remove noise without introducing to much delay
        // Shift history
        memcpy(former_xvals + 1, former_xvals, sizeof(uint16_t) * (length - 1));
        former_xvals[0] = get_adc();

        // Calculate average
        pot_val = 0;
        uint8_t i;
        for(i = 0; i < length; i++)
            pot_val += former_xvals[i];

        pot_val /= length;

        vTaskDelay(10 / portTICK_RATE_MS);
    }
}
