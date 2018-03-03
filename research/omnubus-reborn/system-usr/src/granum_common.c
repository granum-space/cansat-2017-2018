#include "stdbool.h"

#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_spi.h"

#include "granum_common.h"

static I2C_HandleTypeDef i2c1 = {// FIXME настройки неточные, если что
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

void GR_IF_MSP_I2C1() {
	__I2C1_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Alternate = GPIO_AF4_I2C1;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOB, &gpio);
}


static I2C_HandleTypeDef i2c2 = {// FIXME настройки неточные, если что
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

void GR_IF_MSP_I2C2() {
	__I2C2_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Alternate = GPIO_AF4_I2C2;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = GPIO_PIN_10 | GPIO_PIN_11;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOB, &gpio);
}


static I2C_HandleTypeDef i2c3 = {// FIXME настройки неточные, если что
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

void GR_IF_MSP_I2C3() {
	__I2C3_CLK_ENABLE();
	__GPIOH_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Alternate = GPIO_AF4_I2C3;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = GPIO_PIN_7 | GPIO_PIN_8;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOH, &gpio);
}


//SPI
static SPI_HandleTypeDef spi1 = {// FIXME настройки неточные, если что
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

gr_pin_t SPI1_SS = { GPIOA, GPIO_PIN_5 };

void GR_IF_MSP_SPI1() {
	__SPI1_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Alternate = GPIO_AF5_SPI1;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;

	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = SPI1_SS.pin;		//SCK

	HAL_GPIO_Init(SPI1_SS.port, &gpio);

	gpio.Mode = GPIO_MODE_INPUT;
	gpio.Pin = GPIO_PIN_6;		//MISO

	HAL_GPIO_Init(GPIOA, &gpio);

	gpio.Mode = GPIO_MODE_AF_OD;
	gpio.Pin = GPIO_PIN_7;		//MOSI

	HAL_GPIO_Init(GPIOA, &gpio);

	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pin = GPIO_PIN_1;		//SS

	HAL_GPIO_Init(GPIOA, &gpio);
}

gr_interface_t GR_IF_SPI1 = {
	.type = GR_IF_TYPE_SPI,
	.descriptor = &spi1,
	.inited = false,
	.params = {
		.speed = GR_IF_PARAM_SPEED_SLOW,
		.timeout = 1000
	},
	.additional_params = &SPI1_SS,
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

gr_pin_t SPI2_SS = { GPIOA, GPIO_PIN_13 };

void GR_IF_MSP_SPI2() {
	__SPI2_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Alternate = GPIO_AF5_SPI2;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;

	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = SPI2_SS.pin;		//SCK

	HAL_GPIO_Init(SPI2_SS.port, &gpio);

	gpio.Mode = GPIO_MODE_INPUT;
	gpio.Pin = GPIO_PIN_14;		//MISO

	HAL_GPIO_Init(GPIOA, &gpio);

	gpio.Mode = GPIO_MODE_AF_OD;
	gpio.Pin = GPIO_PIN_15;		//MOSI

	HAL_GPIO_Init(GPIOA, &gpio);

	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pin = GPIO_PIN_2;		//SS

	HAL_GPIO_Init(GPIOA, &gpio);
}

gr_interface_t GR_IF_SPI2 = {
	.type = GR_IF_TYPE_SPI,
	.descriptor = &spi2,
	.inited = false,
	.params = {
		.speed = GR_IF_PARAM_SPEED_SLOW,
		.timeout = 1000
	},
	.additional_params = &SPI2_SS,
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

gr_pin_t SPI3_SS = { GPIOB, GPIO_PIN_3 };

void GR_IF_MSP_SPI3() {
	__SPI3_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Alternate = GPIO_AF6_SPI3;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;

	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = SPI3_SS.pin;		//SCK

	HAL_GPIO_Init(SPI3_SS.port, &gpio);

	gpio.Mode = GPIO_MODE_INPUT;
	gpio.Pin = GPIO_PIN_4;		//MISO

	HAL_GPIO_Init(GPIOB, &gpio);

	gpio.Mode = GPIO_MODE_AF_OD;
	gpio.Pin = GPIO_PIN_5;		//MOSI

	HAL_GPIO_Init(GPIOB, &gpio);

	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pin = GPIO_PIN_1;		//SS

	HAL_GPIO_Init(GPIOB, &gpio);
}

gr_interface_t GR_IF_SPI3 = {
	.type = GR_IF_TYPE_SPI,
	.descriptor = &spi3,
	.inited = false,
	.params = {
		.speed = GR_IF_PARAM_SPEED_SLOW,
		.timeout = 1000
	},
	.additional_params = &SPI3_SS,
};
