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

	xSemaphoreHandle mutex;
} gr_interface_t;

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

extern gr_interface_t GR_IF_SPI_1;
#define GR_INTERFACE_SPI_1_PRESC_SLOW SPI_BAUDRATEPRESCALER_128
#define GR_INTERFACE_SPI_1_PRESC_FAST SPI_BAUDRATEPRESCALER_4

extern gr_interface_t GR_IF_SPI_2;
#define GR_INTERFACE_SPI_2_PRESC_SLOW SPI_BAUDRATEPRESCALER_128
#define GR_INTERFACE_SPI_2_PRESC_FAST SPI_BAUDRATEPRESCALER_4

extern gr_interface_t GR_IF_SPI_3;
#define GR_INTERFACE_SPI_3_PRESC_SLOW SPI_BAUDRATEPRESCALER_128
#define GR_INTERFACE_SPI_3_PRESC_FAST SPI_BAUDRATEPRESCALER_4



#endif /* GRANUM_COMMON_H_ */
