#include <stdlib.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_gpio.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "granum_common.h"

#include "devctl.h"


#define ERRORCHECK(EXPR) retval = EXPR; goto end;

static inline gr_error_t _chparams_i2c(gr_interface_t * iface, gr_interface_params_t * params) {
	I2C_HandleTypeDef * descr = iface->descriptor;

	uint32_t newSPEED = 0;

	switch (params->speed) {
		case GR_IF_PARAM_SPEED_SLOW:
			switch ( (size_t) descr->Instance ) {
			case (size_t) SPI1:
				newSPEED = GR_INTERFACE_I2C_1_SPEED_SLOW;
				break;

			case (size_t) SPI2:
				newSPEED = GR_INTERFACE_I2C_2_SPEED_SLOW;
				break;

			case (size_t) SPI3:
				newSPEED = GR_INTERFACE_I2C_3_SPEED_SLOW;
				break;

			default:
				return GR_ERROR_WRONGARG;
			}

			break;

		case GR_IF_PARAM_SPEED_FAST:
			switch ( (size_t) descr->Instance ) {
			case (size_t) SPI1:
				newSPEED = GR_INTERFACE_I2C_1_SPEED_FAST;
				break;

			case (size_t) SPI2:
				newSPEED = GR_INTERFACE_I2C_2_SPEED_FAST;
				break;

			case (size_t) SPI3:
				newSPEED = GR_INTERFACE_I2C_3_SPEED_FAST;
				break;

			default:
				return GR_ERROR_WRONGARG;
			}

			break;

		default:
			return GR_ERROR_WRONGARG;
	}

	HAL_I2C_DeInit(descr); //TODO мб деинитить и не надо?

	descr->Init.ClockSpeed = newSPEED;

	HAL_I2C_Init(descr);

	memcpy(&(iface->params), params, sizeof(gr_interface_params_t) );

	return GR_ERROR_OK;
}

static inline gr_error_t _chparams_spi(gr_interface_t * iface, gr_interface_params_t * params) {
	SPI_HandleTypeDef * descr = iface->descriptor;

	uint32_t newPRESC = 0;

	switch (params->speed) {
		case GR_IF_PARAM_SPEED_SLOW:
			switch ( (size_t) descr->Instance ) {
			case (size_t) SPI1:
				newPRESC = GR_INTERFACE_SPI_1_PRESC_SLOW;
				break;

			case (size_t) SPI2:
				newPRESC = GR_INTERFACE_SPI_2_PRESC_SLOW;
				break;

			case (size_t) SPI3:
				newPRESC = GR_INTERFACE_SPI_3_PRESC_SLOW;
				break;

			default:
				return GR_ERROR_WRONGARG;
			}

			break;

		case GR_IF_PARAM_SPEED_FAST:
			switch ( (size_t) descr->Instance ) {
			case (size_t) SPI1:
				newPRESC = GR_INTERFACE_SPI_1_PRESC_FAST;
				break;

			case (size_t) SPI2:
				newPRESC = GR_INTERFACE_SPI_2_PRESC_FAST;
				break;

			case (size_t) SPI3:
				newPRESC = GR_INTERFACE_SPI_3_PRESC_FAST;
				break;

			default:
				return GR_ERROR_WRONGARG;
			}

			break;

		default:
			return GR_ERROR_WRONGARG;

	}

	HAL_SPI_DeInit(descr); //TODO мб деинитить и не надо?

	descr->Init.BaudRatePrescaler = newPRESC;

	HAL_SPI_Init(descr);

	memcpy(&(iface->params), params, sizeof(gr_interface_params_t) );

	return GR_ERROR_OK;
}

static inline gr_error_t _init_on_need(gr_interface_t * iface) {
	if(!iface->inited) {
		switch(iface->type) {
		case GR_IF_TYPE_I2C:
			return GR_ERROR_FROMHAL( HAL_I2C_Init(iface->descriptor) );

		case GR_IF_TYPE_SPI:
			return GR_ERROR_FROMHAL( HAL_SPI_Init(iface->descriptor) );

		case GR_IF_TYPE_STREAM:
			//there should be stream ifaces handling

		default:
			return GR_ERROR_WRONGARG;
		}
	}

	return GR_ERROR_OK;
}

gr_error_t gr_if_set_params(gr_interface_t * iface, gr_interface_params_t * params) {
	xSemaphoreTake(iface->mutex, 0);

	gr_error_t retval;

	if( !memcmp( &(iface->params), params, sizeof(gr_interface_params_t) ) ) {
		switch(iface->type) {
		case GR_IF_TYPE_I2C:
			ERRORCHECK( _chparams_i2c(iface, params) )

		case GR_IF_TYPE_SPI:
			ERRORCHECK( _chparams_spi(iface, params) )

		case GR_IF_TYPE_STREAM:
			//there should be stream ifaces handling

		default:
			return GR_ERROR_WRONGARG;
		}

	}

end:
	xSemaphoreGive(iface->mutex);
	return GR_ERROR_OK;
}

static inline _readreg_i2c(gr_interface_t * iface, uint8_t dev_addr, uint8_t reg_addr, void * buffer, size_t count) {
	return GR_ERROR_FROMHAL( HAL_I2C_Mem_Read(iface->descriptor, dev_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, buffer, count, iface->params.timeout) );
}

static inline _writereg_i2c(gr_interface_t * iface, uint8_t dev_addr, uint8_t reg_addr, void * buffer, size_t count) {
	return GR_ERROR_FROMHAL( HAL_I2C_Mem_Write(iface->descriptor, dev_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, buffer, count, iface->params.timeout) );
}

static inline _readreg_spi(gr_interface_t * iface, uint8_t dev_addr, uint8_t reg_addr, void * buffer, size_t count) {

}

gr_error_t gr_dev_readreg(gr_interface_t * iface, uint8_t dev_addr, uint8_t reg_addr, void * buffer, size_t count) {
	xSemaphoreTake(iface->mutex, 0);
	gr_error_t retval;

	_init_on_need(iface);

	switch( iface->type ) {
	case GR_IF_TYPE_I2C:
		ERRORCHECK( _readreg_i2c(iface, dev_addr, reg_addr, buffer, count) )

	case GR_IF_TYPE_SPI:
		ERRORCHECK( _readreg_spi(iface, dev_addr, reg_addr, buffer, count) )

	default:
		ERRORCHECK( GR_ERROR_WRONGARG )

	}

end:
	xSemaphoreGive(iface->mutex);
	return retval;
}

gr_error_t gr_dev_writereg(gr_interface_t * iface, uint8_t dev_addr, uint8_t reg_addr, void * buffer, size_t count) {
	xSemaphoreTake(iface->mutex, 0);
	gr_error_t retval;

	_init_on_need(iface);

	switch( iface->type ) {
	case GR_IF_TYPE_I2C:
		ERRORCHECK( _writereg_i2c(iface, dev_addr, reg_addr, buffer, count) )

	case GR_IF_TYPE_SPI:
		ERRORCHECK( _writereg_spi(iface, dev_addr, reg_addr, buffer, count) )

	default:
		ERRORCHECK( GR_ERROR_WRONGARG )
	}

end:
	xSemaphoreGive(iface->mutex);
	return retval;
}
