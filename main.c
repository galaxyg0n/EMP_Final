

/**
 * main.c
 *
 */

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"


void setup()
{

}


int main(void)
{
    setup();

    xTaskCreate();

    vTaskStartScheduler();
	return 0;
}
