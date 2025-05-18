/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: glob_def.h
 * PROJECT: Elevator project
 * DESCRIPTION: Global definitions, enums, and structs used across modules
 *
 * 
 * 
 */

#ifndef HEADERS_GLOB_DEF_H_
#define HEADERS_GLOB_DEF_H_

#include <stdint.h>

/***************** Defines ********************/
#define STR_SIZE 32  // Maximum string size for LCD and other buffers

/***************** Enums **********************/
enum BUTTON_STATES {
    BS_IDLE,  // Button is not pressed
    BS_FP,    // Button was just pressed (First Press)
    BS_LP     // Button is held down (Long Press)
};

enum BUTTON_EVENTS {
    BE_NO,    // No button event
    BE_LONG   // Long press event detected
};

/***************** Structs ********************/
typedef struct {
    uint8_t x;            // X position on LCD (column)
    uint8_t y;            // Y position on LCD (row)
    uint8_t str[STR_SIZE];// String data to display
} LCD_PUT;

typedef struct {
    uint8_t keyPressed;   // Current key pressed on keypad
} KEYPAD_STRUCT;

#endif /* HEADERS_GLOB_DEF_H_ */
