/*
 * uart.c
 *
 *  Created on: 16. mar. 2025
 *      Author: anwup
 */

#include "uart.h"

QueueHandle_t uart_tx_queue;
SemaphoreHandle_t ACC_UPD_MUTEX;
extern double acceleration;

void uart_init(uint32_t baud_rate, uint8_t data_bits, Paritybit parity_bit, uint8_t stop_bits)
{
    //UART0
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;

    //PORTA
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;

    //Alternative funciton
    GPIO_PORTA_AFSEL_R |= (1 << U0Rx) | (1 << U0Tx);

    // set PA1 (uart0 tx) to output
    GPIO_PORTA_DIR_R   |= 0x00000002;
    // set PA0 (uart0 rx) to input
    GPIO_PORTA_DIR_R   &= 0xFFFFFFFE;
    // enable digital operation of PA0 and PA1
    GPIO_PORTA_DEN_R   |= 0x00000003;

    //Configure the port to UART
    GPIO_PORTA_PCTL_R |= (PMC_UART << (4 * U0Rx));
    GPIO_PORTA_PCTL_R |= (PMC_UART << (4 * U0Tx));

    //disable UART during config
    UART0_CTL_R &= ~(1 << UARTEN);


    set_baud_rate(baud_rate);
    set_parity(parity_bit);
    set_data_bits(data_bits);
    set_stop_bits(stop_bits);

    enable_FIFO();

    //System clock
    UART0_CC_R &= ~0x01;

    //Enable UART and TX-mode
    UART0_CTL_R |= (1 << UARTEN) | (1 << TXE) | (1 << RXE);
}

void set_baud_rate(uint32_t baud_rate)
{
    //X-sys*64/(16*baudrate) = 16M*4/baudrate

    uint32_t BRD = configCPU_CLOCK_HZ * 4 / baud_rate;
    UART0_IBRD_R = BRD / 64;
    UART0_FBRD_R = BRD & 0x0000003F;
}

void set_parity(Paritybit parity)
{
    switch(parity)
    {
        case(ODD_PARITY):
                UART0_LCRH_R |= 0x02;
                break;
        case(EVEN_PARITY):
                UART0_LCRH_R |= 0x06;
                break;
        case(NO_PARITY):
        default:
            UART0_LCRH_R |= 0x00;
    }
    UART0_LCRH_R |= parity;
}

void set_data_bits(uint8_t data_bits)
{
    data_bits = data_bits < 5 ? 5 : data_bits;
    data_bits = data_bits > 8 ? 8 : data_bits;

    UART0_LCRH_R |=((uint32_t)data_bits - 5 ) << 5 ;
}

void set_stop_bits(uint8_t stop_bits)
{
    if (stop_bits == 2)
        UART0_LCRH_R |= 0x08;
    else
        UART0_LCRH_R |= 0x0;
}


void uart_write_byte(char data)
{
    while(tx_data_ready());
    UART0_DR_R = data;
}

void uart_write(char* data)
{
    uint8_t i = 0;
    for (i = 0; data[i] != '\0'; i++)
    {
        uart_write_byte(data[i]);
    }
}

char uart_read_byte()
{
    char data;
    while (!rx_data_ready());
    data = UART0_DR_R;
    return data;
}

void uart_read(char* str, char end_char)
{
    char data;
    uint8_t i = 0;
    while (1)
    {
        data = uart_read_byte();
        uart_write_byte(data);
        if (data == end_char)
        {
            break;
        }
        str[i++] = data;
    }
}

bool rx_data_ready()
{
    return !(UART0_FR_R & (1 << RXFE));
}

bool tx_data_ready()
{
    return !(UART0_FR_R & (1 << TXFE));
}

void enable_FIFO()
{
    UART0_LCRH_R  |= (1 << FEN);
}

void disable_FIFO()
{
    UART0_LCRH_R  &= ~(1 << FEN);
}


void uart_tx_task(void *pvParameters)
{
    uart_tx_queue = xQueueCreate(UART_QUEUE_LEN,
                                 sizeof(char)*32);
    configASSERT(uart_tx_queue); //Checks if the queue was created correctly

    char txstr[32];
    while(1)
    {
        if (xQueueReceive(uart_tx_queue, txstr, 1) == pdPASS)
        {
            uart_write(txstr);
        }
    }
}

void uart_rx_task(void *pvParameters)
{
    char rxchar[32];
    char echo_msg[32];
    const char inc_cmd[] = "inc";
    const char dec_cmd[] = "dec";
    const char cha_cmd[] = "cha";

    while(1)
    {
        uart_read(rxchar,13);
        if ((strcmp(rxchar,inc_cmd) == rxchar[(uint8_t)sizeof(inc_cmd)-1]))
        {
            xSemaphoreTake(ACC_UPD_MUTEX,portMAX_DELAY);
            acceleration += ACC_CHANGE;
            snprintf(echo_msg,sizeof(echo_msg),"Acc. increased to:%d.%d f/s^2\n\0",(uint8_t)(acceleration),((uint8_t)(acceleration*10))%10);
            xSemaphoreGive(ACC_UPD_MUTEX);
        }
        if ((strcmp(rxchar,dec_cmd) == rxchar[(uint8_t)sizeof(dec_cmd)-1]))
        {
            xSemaphoreTake(ACC_UPD_MUTEX,portMAX_DELAY);
            acceleration -= ACC_CHANGE;
            snprintf(echo_msg,sizeof(echo_msg),"Acc. decreased to:%d.%d f/s^2\n\0",(uint8_t)(acceleration),((uint8_t)(acceleration*10))%10);
            xSemaphoreGive(ACC_UPD_MUTEX);
        }
        if ((strcmp(rxchar,cha_cmd) == rxchar[(uint8_t)sizeof(cha_cmd)-1]))
        {
            xSemaphoreTake(ACC_UPD_MUTEX,portMAX_DELAY);
            uint8_t change_amount = rxchar[(uint8_t)sizeof(cha_cmd)]-'0';
            if (change_amount<=9 || change_amount>=0)
                acceleration = change_amount;
            snprintf(echo_msg,sizeof(echo_msg),"Acc. changed to:%d.%d f/s^2\n\0",(uint8_t)(acceleration),((uint8_t)(acceleration*10))%10);
            xSemaphoreGive(ACC_UPD_MUTEX);
        }
        uart_queue_put(echo_msg);
    }

}

bool uart_queue_put(char *str)
{
    char queueStr[32];
    memcpy(queueStr, str, sizeof(char)*32);
    return xQueueSendToBack(uart_tx_queue, queueStr, 10) == pdTRUE;
}









