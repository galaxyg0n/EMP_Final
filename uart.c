/***************** Source File ****************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: uart.c
 * PROJECT: Elevator project
 * DESCRIPTION: UART driver and task implementation
 */

#include "uart.h"

/***************** External Variables ****************/
SemaphoreHandle_t ACC_UPD_MUTEX;
extern QueueHandle_t UART_TX_Q;
extern double acceleration;

/***************** Function Definitions ****************/

void init_uart(uint32_t baud_rate, uint8_t data_bits, PARITY_BIT parity_bit, uint8_t stop_bits)
/************************************
* Input   : baud_rate - UART baud rate
*           data_bits - Number of data bits (5-8)
*           parity_bit - Parity setting (NONE, EVEN, ODD)
*           stop_bits - Number of stop bits (1 or 2)
* Output  : None
* Function: Initializes UART0 with specified settings
************************************/
{
    // Enable UART0 and GPIOA clocks
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;

    // Configure PA0 and PA1 for UART
    GPIO_PORTA_AFSEL_R |= (1 << U0Rx) | (1 << U0Tx);
    GPIO_PORTA_DIR_R |= 0x02;      // PA1 TX output
    GPIO_PORTA_DIR_R &= ~0x01;     // PA0 RX input
    GPIO_PORTA_DEN_R |= 0x03;      // Digital enable PA0, PA1

    GPIO_PORTA_PCTL_R |= (PMC_UART << (4 * U0Rx));
    GPIO_PORTA_PCTL_R |= (PMC_UART << (4 * U0Tx));

    UART0_CTL_R &= ~(1 << UARTEN); // Disable UART during config

    set_baud_rate(baud_rate);
    set_parity(parity_bit);
    set_data_bits(data_bits);
    set_stop_bits(stop_bits);

    enable_FIFO();

    UART0_CC_R &= ~0x01; // Use system clock
    UART0_CTL_R |= (1 << UARTEN) | (1 << TXE) | (1 << RXE); // Enable UART, TX and RX
}

void set_baud_rate(uint32_t baud_rate)
/************************************
* Input   : baud_rate - Desired baud rate
* Output  : None
* Function: Sets the baud rate for UART0
************************************/
{
    uint32_t BRD = configCPU_CLOCK_HZ * 4 / baud_rate;
    UART0_IBRD_R = BRD / 64;
    UART0_FBRD_R = BRD & 0x3F;
}

void set_parity(PARITY_BIT parity)
/************************************
* Input   : parity - Parity setting (NONE, EVEN, ODD)
* Output  : None
* Function: Configures UART0 parity bits
************************************/
{
    // Clear previous parity bits
    UART0_LCRH_R &= ~(0x06);

    switch(parity)
    {
        case ODD_PARITY:
            UART0_LCRH_R |= 0x02;
            break;
        case EVEN_PARITY:
            UART0_LCRH_R |= 0x06;
            break;
        case NO_PARITY:
        default:
            UART0_LCRH_R |= 0x00;
    }
}

void set_data_bits(uint8_t data_bits)
/************************************
* Input   : data_bits - Number of data bits (5 to 8)
* Output  : None
* Function: Sets data bits for UART0
************************************/
{
    data_bits = (data_bits < 5) ? 5 : (data_bits > 8 ? 8 : data_bits);

    // Clear previous data bits
    UART0_LCRH_R &= ~(0x60);

    UART0_LCRH_R |= ((data_bits - 5) << 5);
}

void set_stop_bits(uint8_t stop_bits)
/************************************
* Input   : stop_bits - Number of stop bits (1 or 2)
* Output  : None
* Function: Sets the stop bits for UART0
************************************/
{
    if (stop_bits == 2)
        UART0_LCRH_R |= 0x08;
    else
        UART0_LCRH_R &= ~0x08;
}

void uart_write_byte(char data)
/************************************
* Input   : data - Byte to transmit
* Output  : None
* Function: Transmits one byte via UART0
************************************/
{
    while (!tx_data_ready());
    UART0_DR_R = data;
}

void uart_write(char* data)
/************************************
* Input   : data - Null-terminated string to transmit
* Output  : None
* Function: Transmits a null-terminated string via UART0
************************************/
{
    uint8_t i = 0;
    while (data[i] != '\0')
        uart_write_byte(data[i++]);
}

char uart_read_byte()
/************************************
* Input   : None
* Output  : Received character
* Function: Reads one byte from UART0 (blocking)
************************************/
{
    while (!rx_data_ready());
    return UART0_DR_R;
}

void uart_read(char* str, char end_char)
/************************************
* Input   : str - Buffer to store received string
*           end_char - Character indicating end of reception
* Output  : None
* Function: Reads characters into a buffer until end_char is received
************************************/
{
    char data;
    uint8_t i = 0;
    while (1)
    {
        data = uart_read_byte();
        uart_write_byte(data); // Echo back
        if (data == end_char)
            break;
        str[i++] = data;
    }
    str[i] = '\0'; // Null-terminate string
}

bool rx_data_ready()
/************************************
* Input   : None
* Output  : true if UART RX data available, false otherwise
* Function: Checks if UART RX data is ready
************************************/
{
    return !(UART0_FR_R & (1 << RXFE));
}

bool tx_data_ready()
/************************************
* Input   : None
* Output  : true if UART TX buffer is ready to accept data, false otherwise
* Function: Checks if UART TX buffer is ready
************************************/
{
    return !(UART0_FR_R & (1 << TXFF));
}

void enable_FIFO()
/************************************
* Input   : None
* Output  : None
* Function: Enables UART FIFO
************************************/
{
    UART0_LCRH_R |= (1 << FEN);
}

void disable_FIFO()
/************************************
* Input   : None
* Output  : None
* Function: Disables UART FIFO
************************************/
{
    UART0_LCRH_R &= ~(1 << FEN);
}

void uart_tx_task(void *pvParameters)
/************************************
* Input   : pvParameters - Task parameters (unused)
* Output  : None
* Function: UART transmission task; pulls strings from queue and transmits them
************************************/
{
    char txstr[STR_SIZE];
    while (1)
    {
        if (xQueueReceive(UART_TX_Q, txstr, portMAX_DELAY) == pdPASS)
            uart_write(txstr);
    }
}

void uart_rx_task(void *pvParameters)
/************************************
* Input   : pvParameters - Task parameters (unused)
* Output  : None
* Function: UART reception task; parses commands "inc", "dec", "cha" and updates acceleration
************************************/
{
    char rxchar[STR_SIZE];
    char echo_msg[STR_SIZE];
    const char inc_cmd[] = "inc";
    const char dec_cmd[] = "dec";
    const char cha_cmd[] = "cha";

    while (1)
    {
        uart_read(rxchar, 13); // 13 = Enter key (carriage return)

        if (strcmp(rxchar, inc_cmd) == 0)
        {
            xSemaphoreTake(ACC_UPD_MUTEX, portMAX_DELAY);
            acceleration += ACC_CHANGE;
            snprintf(echo_msg, sizeof(echo_msg),
                     "Acc. increased to:%d.%d f/s^2\n",
                     (uint8_t)(acceleration),
                     ((uint8_t)(acceleration * 10)) % 10);
            xSemaphoreGive(ACC_UPD_MUTEX);
        }
        else if (strcmp(rxchar, dec_cmd) == 0)
        {
            xSemaphoreTake(ACC_UPD_MUTEX, portMAX_DELAY);
            acceleration -= ACC_CHANGE;
            snprintf(echo_msg, sizeof(echo_msg),
                     "Acc. decreased to:%d.%d f/s^2\n",
                     (uint8_t)(acceleration),
                     ((uint8_t)(acceleration * 10)) % 10);
            xSemaphoreGive(ACC_UPD_MUTEX);
        }
        else if (strncmp(rxchar, cha_cmd, sizeof(cha_cmd)-1) == 0)
        {
            xSemaphoreTake(ACC_UPD_MUTEX, portMAX_DELAY);
            uint8_t change_amount = rxchar[sizeof(cha_cmd)-1] - '0';
            if (change_amount <= 9)
                acceleration = change_amount;
            snprintf(echo_msg, sizeof(echo_msg),
                     "Acc. changed to:%d.%d f/s^2\n",
                     (uint8_t)(acceleration),
                     ((uint8_t)(acceleration * 10)) % 10);
            xSemaphoreGive(ACC_UPD_MUTEX);
        }

        uart_queue_put(echo_msg);
    }
}

bool uart_queue_put(char *str)
/************************************
* Input   : str - String to enqueue for UART transmission
* Output  : true if string was successfully enqueued, false otherwise
* Function: Sends a string to the UART TX queue
************************************/
{
    char queueStr[STR_SIZE];
    memcpy(queueStr, str, sizeof(char) * STR_SIZE);
    return xQueueSendToBack(UART_TX_Q, queueStr, 10) == pdTRUE;
}
