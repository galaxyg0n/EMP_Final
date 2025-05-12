/*
 * glob_def.h
 *
 *  Created on: 5. maj 2025
 *      Author: karlj
 */

#ifndef HEADERS_GLOB_DEF_H_
#define HEADERS_GLOB_DEF_H_

#include <stdint.h>

typedef struct {
  uint8_t x;
  uint8_t y;
  uint8_t str[32];
} LCD_Put;


enum BUTTON_STATES {BS_IDLE,BS_FP,BS_LP};
enum BUTTON_EVENTS {BE_NO, BE_LONG};

typedef struct {
    uint8_t keyPressed;
} keypadStruct;


#endif /* HEADERS_GLOB_DEF_H_ */
