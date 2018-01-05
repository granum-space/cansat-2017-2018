/*
 * board_ctl.c
 *
 *  Created on: 5 янв. 2018 г.
 *      Author: snork
 */

#include "board_ctl.h"

#include <stdbool.h>

#include "stm32f4xx_hal.h"

//static SPI_HandleTypeDef _spi1;
//static SPI_HandleTypeDef _spi2;
static SPI_HandleTypeDef _spi3 = {
		.Instance = SPI3
};

#define BMP280_SPI (&_spi3)

inline static void _bmp280_cs_control(bool state)
{
	if (state)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
}


void brctl_init(void)
{
	// все пины настраваются в hal_msp, тут явно настраиваем всякую фигню

	// Настраиваем SPI3 для BMP280
	_spi3.Init.Mode = SPI_MODE_MASTER;
	_spi3.Init.Direction = SPI_DIRECTION_2LINES;
	_spi3.Init.DataSize = SPI_DATASIZE_8BIT;
	_spi3.Init.CLKPolarity = SPI_POLARITY_HIGH; // CPOL = 1
	_spi3.Init.CLKPhase = SPI_PHASE_2EDGE;		// CPHA = 1
	_spi3.Init.NSS = SPI_NSS_SOFT;
	_spi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;//SPI_BAUDRATEPRESCALER_8;
	_spi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
	_spi3.Init.TIMode = SPI_TIMODE_DISABLE;
	_spi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	_spi3.Init.CRCPolynomial = 0x0007;
	HAL_SPI_Init(&_spi3);

	// поднимаем CS
	_bmp280_cs_control(false);
}


int8_t brctl_bmp280_reg_read(uint8_t dev_addr, uint8_t reg_addr_, uint8_t *reg_data, uint8_t cnt)
{
	(void)dev_addr;
	volatile uint8_t reg_addr = reg_addr_;
	reg_addr |= (1 << 7); // ставим бит чтения в адрес
	HAL_StatusTypeDef status = HAL_OK;

	_bmp280_cs_control(true);

	// Передаем адрес регистра
	status = HAL_SPI_Transmit(BMP280_SPI, &reg_addr, 1, HAL_MAX_DELAY);
	if (status) goto end;

	// Читаем запрошенный регион
	status = HAL_SPI_Receive(BMP280_SPI, reg_data, cnt, HAL_MAX_DELAY);
	if (status) goto end;

end:
	_bmp280_cs_control(false);
	return status;
}


int8_t brctl_bmp280_reg_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
	(void)dev_addr;
	HAL_StatusTypeDef status = HAL_OK;
	reg_addr &= ~(1<<7);

	_bmp280_cs_control(true);

	for (size_t i = 0; i < cnt; i++)
	{
		// Передаем адрес регистра
		status = HAL_SPI_Transmit(BMP280_SPI, &reg_addr, 1, HAL_MAX_DELAY);
		if (status) goto end;

		// FIXME: Оформить как запись одного шестнадцатибитного инта?
		// короче поереже вызывать тяжелые халовские функции

		// Читаем очередной байт регистра
		status = HAL_SPI_Transmit(BMP280_SPI, reg_data + i, 1, HAL_MAX_DELAY);
		if (status) goto end;
	}

	end:
	_bmp280_cs_control(false);
	return status;
}


