#include "stdbool.h"

#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_spi.h"

#include "granum_common.h"

I2C_HandleTypeDef i2c1 = {// FIXME настройки неточные, если что
	.Instance = I2C1,

	.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT,
	.Init.ClockSpeed = 100000,
	.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE,
	.Init.DutyCycle = I2C_DUTYCYCLE_2,
	.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE,
	.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE,
	.Init.OwnAddress1 = 0xFF,
	.Init.OwnAddress2 = 0xF8,
};

gr_interface_t GR_IF_I2C_1 = {
	.type = GR_IF_TYPE_I2C,
	.descriptor = &i2c1,
	.inited = false,
	.params = {
		.speed = GR_IF_PARAM_SPEED_SLOW,
		.timeout = 1000
	}
};


I2C_HandleTypeDef i2c2 = {// FIXME настройки неточные, если что
	.Instance = I2C2,

	.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT,
	.Init.ClockSpeed = 100000,
	.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE,
	.Init.DutyCycle = I2C_DUTYCYCLE_2,
	.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE,
	.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE,
	.Init.OwnAddress1 = 0xFF,
	.Init.OwnAddress2 = 0xF8,
};

gr_interface_t GR_IF_I2C_2 = {
	.type = GR_IF_TYPE_I2C,
	.descriptor = &i2c2,
	.inited = false,
	.params = {
		.speed = GR_IF_PARAM_SPEED_SLOW,
		.timeout = 1000
	}
};

I2C_HandleTypeDef i2c3 = {// FIXME настройки неточные, если что
	.Instance = I2C3,

	.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT,
	.Init.ClockSpeed = 100000,
	.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE,
	.Init.DutyCycle = I2C_DUTYCYCLE_2,
	.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE,
	.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE,
	.Init.OwnAddress1 = 0xFF,
	.Init.OwnAddress2 = 0xF8,
};

gr_interface_t GR_IF_I2C_3 = {
	.type = GR_IF_TYPE_I2C,
	.descriptor = &i2c3,
	.inited = false,
	.params = {
		.speed = GR_IF_PARAM_SPEED_SLOW,
		.timeout = 1000
	}
};

//SPI
SPI_HandleTypeDef spi1 = {// FIXME настройки неточные, если что
	.Instance = SPI1,

	.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128, //FIXME, Фюрстпримас, ПОДУМОЙ!
	.Init.CLKPhase = SPI_PHASE_1EDGE,
	.Init.CLKPolarity = SPI_POLARITY_HIGH,
	.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE,
	.Init.CRCPolynomial = 0,
	.Init.DataSize = SPI_DATASIZE_8BIT,
	.Init.Direction = SPI_DIRECTION_2LINES,
	.Init.FirstBit = SPI_FIRSTBIT_LSB,
	.Init.Mode = SPI_MODE_MASTER,
	.Init.NSS = SPI_NSS_SOFT,
	.Init.TIMode = SPI_TIMODE_DISABLE,
};

gr_interface_t GR_IF_SPI_1 = {
	.type = GR_IF_TYPE_SPI,
	.descriptor = &spi1,
	.inited = false,
	.params = {
		.speed = GR_IF_PARAM_SPEED_SLOW,
		.timeout = 1000
	}
};


SPI_HandleTypeDef spi2 = {// FIXME настройки неточные, если что
	.Instance = SPI2,

	.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128, //FIXME, Фюрстпримас, ПОДУМОЙ!
	.Init.CLKPhase = SPI_PHASE_1EDGE,
	.Init.CLKPolarity = SPI_POLARITY_HIGH,
	.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE,
	.Init.CRCPolynomial = 0,
	.Init.DataSize = SPI_DATASIZE_8BIT,
	.Init.Direction = SPI_DIRECTION_2LINES,
	.Init.FirstBit = SPI_FIRSTBIT_LSB,
	.Init.Mode = SPI_MODE_MASTER,
	.Init.NSS = SPI_NSS_SOFT,
	.Init.TIMode = SPI_TIMODE_DISABLE,
};

gr_interface_t GR_IF_SPI_2 = {
	.type = GR_IF_TYPE_SPI,
	.descriptor = &spi2,
	.inited = false,
	.params = {
		.speed = GR_IF_PARAM_SPEED_SLOW,
		.timeout = 1000
	}
};

SPI_HandleTypeDef spi3 = {// FIXME настройки неточные, если что
	.Instance = SPI3,

	.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128, //FIXME, Фюрстпримас, ПОДУМОЙ!
	.Init.CLKPhase = SPI_PHASE_1EDGE,
	.Init.CLKPolarity = SPI_POLARITY_HIGH,
	.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE,
	.Init.CRCPolynomial = 0,
	.Init.DataSize = SPI_DATASIZE_8BIT,
	.Init.Direction = SPI_DIRECTION_2LINES,
	.Init.FirstBit = SPI_FIRSTBIT_LSB,
	.Init.Mode = SPI_MODE_MASTER,
	.Init.NSS = SPI_NSS_SOFT,
	.Init.TIMode = SPI_TIMODE_DISABLE,
};

gr_interface_t GR_IF_SPI_3 = {
	.type = GR_IF_TYPE_SPI,
	.descriptor = &spi3,
	.inited = false,
	.params = {
		.speed = GR_IF_PARAM_SPEED_SLOW,
		.timeout = 1000
	}
};
