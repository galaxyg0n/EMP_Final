/*
 * uart.h
 *
 *  Created on: 16. mar. 2025
 *      Author: anwup
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "Task.h"
#include "semphr.h"

#include <string.h>
#include "small_sprints.h"
#include "tm4c123gh6pm.h"
//#include "clock.h"

#define UART_SYS_CLK 16000000UL
#define CLKDIV 16

#define U0Rx 0
#define U0Tx 1

#define PMC_UART 1

#define UARTEN 0
#define TXE 8
#define RXE 9

//UARTLCRH
#define BRK 0 //Send break
#define PEN 1 //Parity enable
#define EPS 2 //Even parity select
#define STP2 3 //2 stop bits select
#define FEN 4 // Enable fifo
#define WLEN 5 //Word length [Bit 5-6]
#define SPS 7 //Stick Parity Select

//UARTFR
#define BUSY 3 //Busy flag
#define RXFE 4 //Receive FIFO empty
#define TXFF 5 //Transmit FIFO Full
#define RXFF 6 //Receive FIFO Full
#define TXFE 7 //Transmit FIFO Empty

#define UART_QUEUE_LEN 20

#define ACC_CHANGE 0.1

typedef enum {NO_PARITY,
              ODD_PARITY,
              EVEN_PARITY} Paritybit;



void uart_init(uint32_t baudrate, uint8_t databits, Paritybit paritybit, uint8_t stopbits);

void set_baud_rate(uint32_t baud_rate);
void set_parity(Paritybit parity);
void set_data_bits(uint8_t data_bits);
void set_stop_bits(uint8_t stop_bits);

void uart_write_byte(char data);

void uart_write(char* data);

void enable_FIFO();

void disable_FIFO();

char uart_read_byte();
void uart_read(char* str, char end_char);

bool rx_data_ready();
bool tx_data_ready();

void uart_tx_task(void *pvParameters);
void uart_rx_task(void *pvParameters);
bool uart_queue_put(char *str);


#endif /* UART_H_ */
