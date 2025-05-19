/***************** Header File ****************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: uart.h
 * PROJECT: Elevator project
 * DESCRIPTION: UART driver interface
 *
 * 
 * 
 * 
 */

#ifndef UART_H_
#define UART_H_

/***************** Includes ****************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

#include "glob_def.h"
#include "small_sprints.h"
#include "tm4c123gh6pm.h"

/***************** Defines ****************/
#define UART_SYS_CLK    16000000UL
#define CLKDIV          16

#define U0Rx            0
#define U0Tx            1
#define PMC_UART        1

#define UARTEN          0
#define TXE             8
#define RXE             9

// UART Line Control (LCRH) bits
#define BRK             0   // Send break
#define PEN             1   // Parity enable
#define EPS             2   // Even parity select
#define STP2            3   // 2 stop bits select
#define FEN             4   // Enable FIFO
#define WLEN            5   // Word length [Bit 5-6]
#define SPS             7   // Stick Parity Select

// UART Flag Register (FR) bits
#define BUSY            3   // Busy flag
#define RXFE            4   // Receive FIFO empty
#define TXFF            5   // Transmit FIFO full
#define RXFF            6   // Receive FIFO full
#define TXFE            7   // Transmit FIFO empty

#define ACC_CHANGE      0.1
#define CARRIAGE_RETURN 13

/***************** Types ****************/
typedef enum {
    NO_PARITY,
    ODD_PARITY,
    EVEN_PARITY
} PARITY_BIT;

/***************** Function Prototypes ****************/

// Initialization
void init_uart(uint32_t baudrate, uint8_t databits, PARITY_BIT paritybit, uint8_t stopbits);

// Configuration
void set_baud_rate(uint32_t baud_rate);
void set_parity(PARITY_BIT parity);
void set_data_bits(uint8_t data_bits);
void set_stop_bits(uint8_t stop_bits);

// Transmit / Receive
void uart_write_byte(char data);
void uart_write(char* data);
char uart_read_byte();
void uart_read(char* str, char end_char);

// FIFO Control
void enable_FIFO();
void disable_FIFO();

// Status
bool rx_data_ready();
bool tx_data_ready();

// Tasks
void uart_tx_task(void *pvParameters);
void uart_rx_task(void *pvParameters);

// Queue
bool uart_queue_put(char *str);

#endif /* UART_H_ */
