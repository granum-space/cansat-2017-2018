#include "stm32f4xx_hal.h"


static void _spi3_init(SPI_HandleTypeDef * hspi)
{
	(void)hspi;

	// SPI3 находится на порту C
	__HAL_RCC_GPIOC_CLK_ENABLE(); // FIXME: Нигде не выключается

	GPIO_InitTypeDef bmp_spi_p_init;
	bmp_spi_p_init.Alternate = GPIO_AF6_SPI3;
	bmp_spi_p_init.Pull = GPIO_NOPULL;
	bmp_spi_p_init.Speed = GPIO_SPEED_HIGH;

	// SCK
	bmp_spi_p_init.Pull = GPIO_PULLUP; // из-за полярности клока CPOL = 1
	bmp_spi_p_init.Mode = GPIO_MODE_AF_PP;
	bmp_spi_p_init.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOC, &bmp_spi_p_init);

	// MOSI
	bmp_spi_p_init.Pin = GPIO_PIN_12;
	bmp_spi_p_init.Mode = GPIO_MODE_AF_PP;
	bmp_spi_p_init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &bmp_spi_p_init);

	// MISO
	bmp_spi_p_init.Pin = GPIO_PIN_11;
	bmp_spi_p_init.Mode = GPIO_MODE_AF_PP; // AF_PP? Хм
	bmp_spi_p_init.Pull = GPIO_PULLUP;// GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &bmp_spi_p_init);

	// NSS (A15). Не настраиваем?

	// FIXME: Убрать. Поставим в 1, чтобы проверить что что-то читается с реального пина
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);

	// Включаем сам SPI
	__HAL_RCC_SPI3_CLK_ENABLE();
}


static void _spi3_deinit(void)
{
	__HAL_RCC_SPI3_FORCE_RESET();
	__HAL_RCC_SPI3_RELEASE_RESET();

	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
}


void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	if (SPI3 == hspi->Instance)
		_spi3_init(hspi);
}


void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
	if (SPI3 == hspi->Instance)
		_spi3_deinit();
}
