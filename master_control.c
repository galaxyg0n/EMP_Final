/*
 * master_control.c
 *
 *  Created on: 5. maj 2025
 *      Author: karlj
 */

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "lcd.h"
#include "glob_def.h"


void master_control_task(void* pvParameters)
{
    static enum CONT_STATES {E_STILL,E_PASS,E_RUN,E_BROKEN} cont_state = E_STILL;
    while(1)
    {
        switch(cont_state)
        {
        case E_STILL:
            break;
        case E_RUN:
            break;
        case E_BROKEN:
            break;
        default:
            break;
        }
    }
}



