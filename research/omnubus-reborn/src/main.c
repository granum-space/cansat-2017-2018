#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include <stm32f4xx_hal.h>
//
//
//SPI_HandleTypeDef spi;
//
//void init_hw()
//{
//	spi.Instance               = SPI3;
//
//	spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
//	spi.Init.Direction         = SPI_DIRECTION_2LINES;
//	spi.Init.CLKPhase          = SPI_PHASE_1EDGE;
//	spi.Init.CLKPolarity       = SPI_POLARITY_HIGH;
//	spi.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLED;
//	spi.Init.CRCPolynomial     = 7;
//	spi.Init.DataSize          = SPI_DATASIZE_8BIT;
//	spi.Init.FirstBit          = SPI_FIRSTBIT_MSB;
//	spi.Init.NSS               = SPI_NSS_SOFT;
//	spi.Init.TIMode            = SPI_TIMODE_DISABLED;
//
//	spi.Init.Mode	 = SPI_MODE_MASTER;
//
//}

#include "FreeRTOS.h"
#include "task.h"

#include "board_ctl.h"
#include "bmp280_local.h"

void thetask(void * arg)
{
	(void)arg;

	struct bmp280_t bmp;
	bmp280_local_init(&bmp);
	int res;

	res = bmp280_init(&bmp);
	trace_printf("bmp_init = %d\n", res);

	res = bmp280_set_work_mode(BMP280_ULTRA_HIGH_RESOLUTION_MODE);
	trace_printf("bmp_set_work_mode = %d\n", res);

	res = bmp280_set_power_mode(BMP280_NORMAL_MODE);
	trace_printf("bmp_set_power_mode = %d\n", res);

	bool value = true;
	while(1)
	{
		value = !value;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, value ? GPIO_PIN_SET : GPIO_PIN_RESET);

		int32_t pressure;
		int32_t temperature;
		res = bmp280_read_uncomp_pressure_temperature(&pressure, &temperature);
		double p = bmp280_compensate_pressure_double(pressure);
		double t = bmp280_compensate_temperature_double(temperature);
		printf("res = %d, pres = %lf, temp = %lf\n", res, p, t);

		vTaskDelay(500);
	}
}

static StaticTask_t _taskObj;
static StackType_t _taskStack[configMINIMAL_STACK_SIZE*3];

int main()
{
	brctl_init();
	xTaskCreateStatic(thetask, "task", sizeof(_taskStack)/sizeof(*_taskStack), NULL,
			configMAX_PRIORITIES-1, _taskStack, &_taskObj);

	vTaskStartScheduler();

	return 0;
}
