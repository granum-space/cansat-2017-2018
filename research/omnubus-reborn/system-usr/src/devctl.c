#include <stdbool.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_gpio.h"

#include "devctl.h"



struct gr_interface_struct {
	enum {
		GR_IF_TYPE_I2C,
		GR_IF_TYPE_SPI,
		GR_IF_TYPE_STREAM,
	} type;

	void * descriptor;

	bool inited;

	gr_interface_params_t params;
};

I2C_HandleTypeDef i2c1 = {.Instance = I2C1};
gr_interface_t GR_IF_I2C_1 = {.type = GR_IF_TYPE_I2C, .descriptor = &i2c1, .inited = false};

I2C_HandleTypeDef i2c2 = {.Instance = I2C2};
gr_interface_t GR_IF_I2C_2 = {.type = GR_IF_TYPE_I2C, .descriptor = &i2c2, .inited = false};

I2C_HandleTypeDef i2c3 = {.Instance = I2C3};
gr_interface_t GR_IF_I2C_3 = {.type = GR_IF_TYPE_I2C, .descriptor = &i2c3, .inited = false};


SPI_HandleTypeDef spi1 = {.Instance = SPI1};
gr_interface_t GR_IF_SPI_1 = {.type = GR_IF_TYPE_SPI, .descriptor = &spi1, .inited = false};

SPI_HandleTypeDef spi2 = {.Instance = SPI2};
gr_interface_t GR_IF_SPI_2 = {.type = GR_IF_TYPE_SPI, .descriptor = &spi2, .inited = false};

SPI_HandleTypeDef spi3 = {.Instance = SPI3};
gr_interface_t GR_IF_SPI_3 = {.type = GR_IF_TYPE_SPI, .descriptor = &spi3, .inited = false};


static gr_error_t _init_spi(gr_interface_t * iface) {
	if(!iface->inited) {

	}

	return GR_ERROR_OK;
}

static gr_error_t _chparams_spi(gr_interface_t * iface, gr_interface_params_t * params) {


	return GR_ERROR_OK;
}

static gr_error_t _init_i2c(gr_interface_t * iface) {
	return GR_ERROR_OK;
}

static gr_error_t _init_on_need(gr_interface_t * iface) {
	if(!iface->inited) {
		switch(iface->type) {
		case GR_IF_TYPE_I2C:
			return _init_i2c(iface);

		case GR_IF_TYPE_SPI:
			return _init_spi(iface);

		case GR_IF_TYPE_STREAM:
			//there should be stream ifaces handling

		default:
			return GR_ERROR_WRONGARG;
		}
	}

	return GR_ERROR_OK;
}

gr_error_t gr_if_set_params(gr_interface_t * iface, gr_interface_params_t * params) {
	if( !memcmp( &(iface->params), params, sizeof(gr_interface_params_t) ) ) {
		switch(iface->type) {
		case GR_IF_TYPE_I2C:
			return _chparams_i2c(iface, params);

		case GR_IF_TYPE_SPI:
			return _chparams_spi(iface, params);

		case GR_IF_TYPE_STREAM:
			//there should be stream ifaces handling

		default:
			return GR_ERROR_WRONGARG;
		}
	}

	return GR_ERROR_OK;
}
