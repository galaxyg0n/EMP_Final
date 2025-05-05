/***************** Header *********************/
/*
 * Univeristy of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: lcd.c
 * PROJECT: Assignment_3
 * DESCRIPTION: See module specification .h-file
 */

/***************** Includes *******************/
#include "lcd.h"

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
extern QueueHandle_t LCD_Q;


/***************** Functions ******************/
static void toggle_E()
/************************************
*Function: Internal function, toggles the enable bit
************************************/
{
    GPIO_PORTD_DATA_R ^= E; //Toggle the read write enable bit
    GPIO_PORTD_DATA_R ^= E;
    vTaskDelay(5/portTICK_RATE_MS);     //Give the LCD time to process/write
}

static void lcd_write(uint8_t is_data, uint8_t data)
/************************************
*Input   : The data to be written (data), aswell as which register (is_data)
*Function: Internal function. Writes data to the LCD display either DATA register or INSTR register
************************************/
{
    GPIO_PORTD_DATA_R &= ~E; //Disable r/w

    if (is_data)
        GPIO_PORTD_DATA_R |= RS; //Set data write
    else
        GPIO_PORTD_DATA_R &= ~RS; //Set Instr. write

    //Write 4-MSB
    GPIO_PORTC_DATA_R = data; //Put the data on GPIO port C pins
    toggle_E(); //EN r/w then DB

    //Write 4-LSB in the same way
    GPIO_PORTC_DATA_R = (data<<4);
    toggle_E();
}

void lcd_clear()
//See .h-file for function description
{
    lcd_write(INSTR,CLEAR_DISPLAY); //Write the CLEAR DISPLAY instruction
}

void lcd_displaySettings(uint8_t is_on, uint8_t cursor_on, uint8_t blinking_on)
//See .h-file for function description
{
    //Writes the chosen bits at the right positions according to the data sheet
    uint8_t write_val = (1<<DISPLAY_CONTROL)+(is_on<<DISPLAY_BIT_EN)+(cursor_on<<CURSOR_BIT_EN)+(blinking_on<<BLINK_BIT_EN);
    lcd_write(INSTR,write_val);
}

void lcd_entrySettings(uint8_t addrIncr, uint8_t dispShift)
//See .h-file for function description
{
    //Writes the chosen bits at the right positions according to the data sheet
    uint8_t write_val = (1<<ENTRY_CONTROL)+(addrIncr<<INCR_BIT_EN)+(dispShift<<DISP_SHIFT_BIT_EN);
    lcd_write(INSTR,write_val);
}

void lcd_setCursor(uint8_t row, uint8_t column)
//See .h-file for function description
{
    //Writes the chosen bits at the right positions according to the data sheet
    uint8_t position_val = (row-1)*(SECOND_ROW)+(column-1);
    uint8_t write_val = (1<<DDRAM_BIT_EN)+position_val;
    lcd_write(INSTR,write_val);
}

void lcd_print_char(uint8_t char_to_print)
//See .h-file for function description
{
    lcd_write(DATA,char_to_print);
}

void lcd_print_str(uint8_t* str)
//See .h-file for function description
{
    do //Loops through char array and prints each
    {
        lcd_print_char(*str); //Prints char
    }while(*(++str));
}

void init_lcd()
//See .h-file for function description
{
    //General pin init.
    int dummy;
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOC + SYSCTL_RCGC2_GPIOD + SYSCTL_RCGC2_GPIOF; //Enable clock on pins
    dummy = SYSCTL_RCGC2_R;

    //Sets data direction
    GPIO_PORTC_DIR_R |= LCD_DATA_PINS;
    GPIO_PORTD_DIR_R |= RS+E;
    GPIO_PORTC_DEN_R |= LCD_DATA_PINS;
    GPIO_PORTD_DEN_R |= RS+E;

    //Specific init. for 4-bit data transfer
    GPIO_PORTD_DATA_R &= ~(E); //DB r/w
    GPIO_PORTC_DATA_R |= (1<<FS); //Function select 4-bit fct.
    GPIO_PORTD_DATA_R &= ~(RS); //Select instr. reg.


    vTaskDelay(100/portTICK_RATE_MS); //Wait for 100ms to let LCD start

    //Function select runs twice to init. 4-bit
    toggle_E();
    lcd_write(INSTR,(1<<FS)+DOUBLE_LINE); //Here LCD is also set to 2X Line and font

    lcd_entrySettings(1,0); //Increment address no display shift
    lcd_displaySettings(1,0,0); //Display on, cursor on, no cursor blinking

    lcd_clear(); //Clear display if something should be on it already
}

void LCD_task(void* pvParameters)
{
    init_lcd();

    static LCD_Put to_write;
    static uint8_t clear_cmd[] = "clc";

    while(1)
    {
        xQueueReceive(LCD_Q,&to_write,portMAX_DELAY);
        if (!strcmp(to_write.str,clear_cmd))
            lcd_clear();
        else
        {
        lcd_setCursor(to_write.y,to_write.x);
        lcd_print_str(to_write.str);
        }
    }
}
