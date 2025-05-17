/*
 * potentiometer.c
 *
 *  Created on: 15 May 2025
 *      Author: mathi
 */


#include "potentiometer.h"

uint16_t pot_val;
// -------------- Stolen from ITSL --------------
uint16_t get_adc()
{
    return( ADC0_SSFIFO3_R );
}

void init_adc()
{
    SYSCTL_RCGC0_R |= SYSCTL_RCGC0_ADC0;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;

    // Set ADC0 Sequencer priorities.
    // SS3(bit12-13): Priority = 0 ; Highest
    // SS2(bit8-9):   Priority = 1
    // SS1(bit4-5):   Priority = 2
    // SS0(bit0-1):   Priority = 3 ; Lowest
    ADC0_SSPRI_R = 0x00000123;

    //Disable all sequencers
    ADC0_ACTSS_R = 0;

    // Trigger for Sequencer 3 (bit 12-15) = 0xF = Always.
    ADC0_EMUX_R = 0x0000F000;

    //sample multiplexer input, sequencer 3 select, ADC 11 (0x0B) enable.
    ADC0_SSMUX3_R = 0x0B;

    //ADC sample sequence control 3 = END0
    ADC0_SSCTL3_R =  0x00000002;

    //enable sequencer 3
    ADC0_ACTSS_R = 0x00000008;

    // Start conversion at sequencer 3
    ADC0_PSSI_R = 0x08;
}
// ---------------------------------------------

void adcvalue_to_string(uint16_t adc_value, char* return_buf, size_t buf_size)
{
    sprintf(return_buf, "%u\n", adc_value);
}


void send_string_uart(const char *str)
{
    while (*str)  // Loop until null terminator
    {
        uart_queue_put(*str++);
    }
}

void potentiometer_task(void* pvParameters)
{
    char buffer[10];
    uint16_t former_xvals[] = {0,0,0};
    const uint8_t length = (uint8_t)(sizeof(former_xvals));
    while(1)
    {
        memcpy(former_xvals+1,former_xvals,sizeof(uint16_t)*(length-1));
        former_xvals[0] = get_adc();
        uint8_t i;
        pot_val = 0;
        for(i = 0; i<length; i++)
            pot_val += former_xvals[i];
        pot_val /= length;

        vTaskDelay(10 / portTICK_RATE_MS);
    }
}
