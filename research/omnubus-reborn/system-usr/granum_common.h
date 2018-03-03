/*
 * granum_common.h
 *
 *  Created on: 10 февр. 2018 г.
 *      Author: developer
 */

#ifndef GRANUM_COMMON_H_
#define GRANUM_COMMON_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#include "FreeRTOS.h"
#include "semphr.h"

typedef struct {
	enum {
		GR_IF_PARAM_SPEED_SLOW,
		GR_IF_PARAM_SPEED_FAST,
	} speed;

	uint32_t timeout;

} gr_interface_params_t;

typedef struct {
	enum {
		GR_IF_TYPE_I2C,
		GR_IF_TYPE_SPI,
		GR_IF_TYPE_STREAM,
	} type;

	void * descriptor;

	bool inited;

	gr_interface_params_t params;
	void * additional_params;

	xSemaphoreHandle mutex;
} gr_interface_t;

typedef struct {
	GPIO_TypeDef * port;
	uint16_t pin;
} gr_pin_t;

typedef union {
	gr_pin_t pin;
	uint8_t address;
} gr_device_selector_t;

#define GR_ERROR_FROMHAL(STATUS) STATUS

typedef enum {
	GR_ERROR_OK,
	GR_ERROR_WRONGARG,
} gr_error_t;

extern gr_interface_t GR_IF_I2C_1;
#define GR_INTERFACE_I2C_1_SPEED_SLOW 100000
#define GR_INTERFACE_I2C_1_SPEED_FAST 400000

extern gr_interface_t GR_IF_I2C_2;
#define GR_INTERFACE_I2C_2_SPEED_SLOW 100000
#define GR_INTERFACE_I2C_2_SPEED_FAST 400000

extern gr_interface_t GR_IF_I2C_3;
#define GR_INTERFACE_I2C_3_SPEED_SLOW 100000
#define GR_INTERFACE_I2C_3_SPEED_FAST 400000

extern gr_interface_t GR_IF_SPI1;
extern gr_pin_t SPI1_SS;
#define GR_INTERFACE_SPI_1_PRESC_SLOW SPI_BAUDRATEPRESCALER_128
#define GR_INTERFACE_SPI_1_PRESC_FAST SPI_BAUDRATEPRESCALER_4

extern gr_interface_t GR_IF_SPI2;
extern gr_pin_t SPI2_SS;
#define GR_INTERFACE_SPI_2_PRESC_SLOW SPI_BAUDRATEPRESCALER_128
#define GR_INTERFACE_SPI_2_PRESC_FAST SPI_BAUDRATEPRESCALER_4

extern gr_interface_t GR_IF_SPI3;
extern gr_pin_t SPI3_SS;
#define GR_INTERFACE_SPI_3_PRESC_SLOW SPI_BAUDRATEPRESCALER_128
#define GR_INTERFACE_SPI_3_PRESC_FAST SPI_BAUDRATEPRESCALER_4

#define BMP280_IF GR_IF_SPI1
#define BMP280_SS SPI1_SS

#endif /* GRANUM_COMMON_H_ */
