/*
 * board-def.h
 *
 *  Created on: 5 янв. 2018 г.
 *      Author: snork
 */

#ifndef BOARD_CTL_H_
#define BOARD_CTL_H_

// Определения как сввязаться с тем, что расположено на плате

#include <stdbool.h>

#include "stm32f4xx_hal.h"

void brctl_init(void);

// Доступ к регистрам BMP280
int8_t brctl_bmp280_reg_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt);
int8_t brctl_bmp280_reg_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt);

void brctl_led_set(bool value);

#endif /* BOARD_CTL_H_ */
