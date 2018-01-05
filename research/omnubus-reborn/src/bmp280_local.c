/*
 * bmp280_local.c
 *
 *  Created on: 5 янв. 2018 г.
 *      Author: snork
 */

#include "bmp280_local.h"

#include "FreeRTOS.h"
#include "task.h"

#include "board_ctl.h"


static void _rtos_delay_ms(uint32_t ms)
{
	vTaskDelay(ms*configTICK_RATE_HZ/1000);
}

void bmp280_local_init(struct bmp280_t * device)
{
	device->bus_read = brctl_bmp280_reg_read;
	device->bus_write = brctl_bmp280_reg_write;
	device->delay_msec = _rtos_delay_ms;
}


