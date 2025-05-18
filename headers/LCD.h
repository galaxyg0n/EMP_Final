/***************** Header *********************/
/*
 * Univeristy of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: lcd.h
 * PROJECT: Assignment_3
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
//Pin numbers
#define LCD_DATA_PINS 0xF0 //Data are on port C
#define RS 0x04 //E and RS are on port D
#define E 0x08

//Data & Instruction
#define DATA 1
#define INSTR 0

//Function bits
#define FS 5 //Function set
#define DOUBLE_LINE 0x0F
#define SINGLE_LINE 0

//Display control bits
#define DISPLAY_CONTROL 3
#define DISPLAY_BIT_EN 2
#define CURSOR_BIT_EN 1
#define BLINK_BIT_EN 0

//Entry mode bits
#define ENTRY_CONTROL 2
#define INCR_BIT_EN 1
#define DISP_SHIFT_BIT_EN 0

//Display clear
#define CLEAR_DISPLAY 0x01

//Cursor positions
#define DDRAM_BIT_EN 7
#define SECOND_ROW 0x40

/***************** Const. *********************/
/***************** Variables ******************/

/***************** Functions ******************/

void lcd_print_char(uint8_t char_to_print);
/************************************
*Input   : Character to be written
*Output  : No output
*Function: Writes the character at current cursor position on LCD
************************************/

void lcd_print_str(uint8_t* str);
/************************************
*Input   : String (Char array) to be written
*Output  : No output
*Function: Writes the string to the LCD
************************************/

void lcd_setCursor(uint8_t row, uint8_t column);
/************************************
*Input   : Row and column number on the LCD (RxC)
*Output  : No output
*Function: Sets the cursor on the LCD to the given position
************************************/

void lcd_entrySettings(uint8_t addrIncr, uint8_t dispShift);
/************************************
*Input   : Values for address increment direction and display shift direction
*Output  : No output
*Function: Choose whether R/L address increment and R/L display shift
************************************/

void lcd_displaySettings(uint8_t is_on, uint8_t cursor_on, uint8_t blinking_on);
/************************************
*Input   : Values for display settings
*Output  : No output
*Function: Chooses if display is on, cursor is on and if blinking is on
************************************/

void lcd_clear();
/************************************
*Input   : No input
*Output  : No output
*Function: Clears all chars from the LCD
************************************/

void init_lcd();
/************************************
*Input   : No input
*Output  : No output
*Function: Initializes the LCD
************************************/

void lcd_task(void* pvParameters);

bool lcd_queue_put(uint8_t x, uint8_t y, uint8_t str[STR_SIZE]);
bool lcd_queue_get(LCD_PUT *returnStruct);

#endif /* LCD_H_ */
