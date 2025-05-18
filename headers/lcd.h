/***************** Header *********************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: lcd.h
 * PROJECT   : Elevator project
 * DESCRIPTION: LCD function library header file
 */

#ifndef LCD_H_
#define LCD_H_

/***************** Includes *******************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "glob_def.h"

/***************** Defines ********************/
// LCD pin configuration
#define LCD_DATA_PINS     0xF0    // Data lines connected to Port C (PC4–PC7)
#define RS                0x04    // Register Select (PD2)
#define E                 0x08    // Enable (PD3)

// Register selection
#define DATA              1
#define INSTR             0

// Function set bits
#define FS                5
#define DOUBLE_LINE       0x0F
#define SINGLE_LINE       0x00

// Display control
#define DISPLAY_CONTROL   3
#define DISPLAY_BIT_EN    2
#define CURSOR_BIT_EN     1
#define BLINK_BIT_EN      0

// Entry mode
#define ENTRY_CONTROL     2
#define INCR_BIT_EN       1
#define DISP_SHIFT_BIT_EN 0

// Display clear
#define CLEAR_DISPLAY     0x01

// DDRAM address set
#define DDRAM_BIT_EN      7
#define SECOND_ROW        0x40

/***************** Constants *******************/
/***************** Variables *******************/
/***************** Functions *******************/

void lcd_print_char(uint8_t char_to_print);
/************************************
* Input   : Character to be written
* Output  : None
* Function: Writes the character at current cursor position on LCD
************************************/

void lcd_print_str(uint8_t* str);
/************************************
* Input   : String (char array) to be written
* Output  : None
* Function: Writes the entire string to the LCD
************************************/

void lcd_setCursor(uint8_t row, uint8_t column);
/************************************
* Input   : LCD row and column position (1-indexed)
* Output  : None
* Function: Sets the cursor position on the LCD
************************************/

void lcd_entrySettings(uint8_t addrIncr, uint8_t dispShift);
/************************************
* Input   : Address increment (1/0), Display shift (1/0)
* Output  : None
* Function: Configures entry mode settings for LCD
************************************/

void lcd_displaySettings(uint8_t is_on, uint8_t cursor_on, uint8_t blinking_on);
/************************************
* Input   : Display on/off, cursor on/off, blink on/off flags
* Output  : None
* Function: Sets LCD display configuration
************************************/

void lcd_clear(void);
/************************************
* Input   : None
* Output  : None
* Function: Clears all characters from the LCD
************************************/

void init_lcd(void);
/************************************
* Input   : None
* Output  : None
* Function: Initializes LCD hardware and display settings
************************************/

void lcd_task(void* pvParameters);
/************************************
* Input   : pvParameters (FreeRTOS task parameter, unused)
* Output  : None
* Function: LCD FreeRTOS task for displaying messages
************************************/

bool lcd_queue_put(uint8_t x, uint8_t y, uint8_t str[STR_SIZE]);
/************************************
* Input   : LCD x/y position, string to display
* Output  : true if successfully queued, false otherwise
* Function: Sends data to the LCD task via FreeRTOS queue
************************************/

bool lcd_queue_get(LCD_PUT *returnStruct);
/************************************
* Input   : Pointer to store received struct
* Output  : true if a message was received, false otherwise
* Function: Reads a message from the LCD queue
************************************/

#endif /* LCD_H_ */
