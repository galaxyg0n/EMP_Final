/***************** Header *********************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: lcd.c
 * PROJECT: Elevator project
 * DESCRIPTION: See module specification in .h file
 */

/***************** Includes *******************/
#include "lcd.h"

/***************** Constants ******************/
/***************** Variables ******************/
extern QueueHandle_t LCD_Q;

/***************** Local Function Declarations ***************/
static void toggle_E(void);
static void lcd_write(uint8_t is_data, uint8_t data);

/***************** Local Functions ***************/
static void toggle_E()
/************************************
* Function : Toggles the LCD enable bit (E)
* Argument : None
* Returns  : None
************************************/
{
    GPIO_PORTD_DATA_R ^= E;      // Toggle enable bit
    GPIO_PORTD_DATA_R ^= E;      // Toggle again to latch
    vTaskDelay(5 / portTICK_RATE_MS); // Wait for LCD to process
}

static void lcd_write(uint8_t is_data, uint8_t data)
/************************************
* Function : Writes to the LCD data or instruction register
* Argument : is_data - 1 if writing data, 0 if writing instruction
*            data    - 8-bit data to write
* Returns  : None
************************************/
{
    GPIO_PORTD_DATA_R &= ~E;           // Disable write
    if (is_data)
        GPIO_PORTD_DATA_R |= RS;       // Select data register
    else
        GPIO_PORTD_DATA_R &= ~RS;      // Select instruction register

    GPIO_PORTC_DATA_R = data;          // Send high nibble
    toggle_E();

    GPIO_PORTC_DATA_R = (data << 4);   // Send low nibble
    toggle_E();
}

/***************** Global Functions ***************/

void lcd_clear()
/************************************
* See .h file for description
************************************/
{
    lcd_write(INSTR, CLEAR_DISPLAY);
}

void lcd_displaySettings(uint8_t is_on, uint8_t cursor_on, uint8_t blinking_on)
/************************************
* See .h file for description
************************************/
{
    uint8_t write_val = (1 << DISPLAY_CONTROL) |
                        (is_on << DISPLAY_BIT_EN) |
                        (cursor_on << CURSOR_BIT_EN) |
                        (blinking_on << BLINK_BIT_EN);
    lcd_write(INSTR, write_val);
}

void lcd_entrySettings(uint8_t addrIncr, uint8_t dispShift)
/************************************
* See .h file for description
************************************/
{
    uint8_t write_val = (1 << ENTRY_CONTROL) |
                        (addrIncr << INCR_BIT_EN) |
                        (dispShift << DISP_SHIFT_BIT_EN);
    lcd_write(INSTR, write_val);
}

void lcd_setCursor(uint8_t row, uint8_t column)
/************************************
* See .h file for description
************************************/
{
    uint8_t position_val = (row - 1) * SECOND_ROW + (column - 1);
    uint8_t write_val = (1 << DDRAM_BIT_EN) + position_val;
    lcd_write(INSTR, write_val);
}

void lcd_print_char(uint8_t char_to_print)
/************************************
* See .h file for description
************************************/
{
    lcd_write(DATA, char_to_print);
}

void lcd_print_str(uint8_t* str)
/************************************
* See .h file for description
************************************/
{
    do
    {
        if (*str == '\n')
            lcd_setCursor(2, 1);
        else
            lcd_print_char(*str);
    } while (*(++str));
}

void init_lcd()
/************************************
* See .h file for description
************************************/
{
    volatile int dummy;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC | SYSCTL_RCGC2_GPIOD | SYSCTL_RCGC2_GPIOF;
    dummy = SYSCTL_RCGC2_R;

    GPIO_PORTC_DIR_R |= LCD_DATA_PINS;
    GPIO_PORTD_DIR_R |= RS | E;
    GPIO_PORTC_DEN_R |= LCD_DATA_PINS;
    GPIO_PORTD_DEN_R |= RS | E;

    GPIO_PORTD_DATA_R &= ~E;
    GPIO_PORTC_DATA_R |= (1 << FS);
    GPIO_PORTD_DATA_R &= ~RS;

    vTaskDelay(100 / portTICK_RATE_MS);

    toggle_E();
    lcd_write(INSTR, (1 << FS) | DOUBLE_LINE);

    lcd_entrySettings(1, 0);
    lcd_displaySettings(1, 0, 0);
    lcd_clear();
}

void lcd_task(void* pvParameters)
/************************************
* Function : LCD display FreeRTOS task
* Argument : pvParameters (not used)
* Returns  : None
************************************/
{
    init_lcd();

    static LCD_PUT to_write;
    const uint8_t clear_cmd[] = "clc";

    while (1)
    {
        xQueueReceive(LCD_Q, &to_write, portMAX_DELAY);
        if (!strcmp(to_write.str, clear_cmd))
            lcd_clear();
        else
        {
            lcd_setCursor(to_write.y, to_write.x);
            lcd_print_str(to_write.str);
        }
    }
}

bool lcd_queue_put(uint8_t x, uint8_t y, uint8_t str[STR_SIZE])
/************************************
* Function : Send string to LCD via FreeRTOS queue
* Argument : x, y  - cursor coordinates
*            str   - string to display
* Returns  : true if successfully queued
************************************/
{
    LCD_PUT queueStruct;
    queueStruct.x = x;
    queueStruct.y = y;
    memcpy(queueStruct.str, str, sizeof(queueStruct.str));

    return xQueueSendToBack(LCD_Q, &queueStruct, 10 / portTICK_RATE_MS) == pdTRUE;
}

bool lcd_queue_get(LCD_PUT *returnStruct)
/************************************
* Function : Retrieve next LCD write command from queue
* Argument : returnStruct - pointer to store retrieved value
* Returns  : true if successful
************************************/
{
    if (LCD_Q != NULL)
    {
        return xQueueReceive(LCD_Q, returnStruct, 10 / portTICK_RATE_MS) == pdTRUE;
    }
    return pdFALSE;
}
