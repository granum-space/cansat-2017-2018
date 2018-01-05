/*
 * hooks.c
 *
 *  Created on: 5 янв. 2018 г.
 *      Author: snork
 */

#include "FreeRTOS.h"
#include "task.h"

#include <stdlib.h>

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	(void)xTask;
	(void)pcTaskName;
	abort();
}


void vApplicationGetIdleTaskMemory(StaticTask_t ** pxIdleTaskTCBBuffer, StackType_t ** pxIdleTaskStackBuffer,
		uint32_t *ulIdleTaskStackSize)
{
	static StaticTask_t task;
	static StackType_t stack[configMINIMAL_STACK_SIZE];

	*pxIdleTaskTCBBuffer = &task;
	*pxIdleTaskStackBuffer = stack;
	*ulIdleTaskStackSize = sizeof(stack)/sizeof(*stack);
}

