/*
 * humidity.h
 *
 *  Created on: 11 мая 2018 г.
 *      Author: developer
 */

#ifndef HUMIDITY_H_
#define HUMIDITY_H_

#include <stdint.h>
#include <stdio.h>

typedef enum {
	AM2320_ERROR_OK = 0,
	AM2320_ERROR_OPEN,
	AM2320_ERRROR_IOCTL,
	AM2320_ERROR_WRITE,
	AM2320_ERROR_READ
} am2320_errors_t;

am2320_errors_t am2320_get_data(float* humidity, float* temperature);

#endif /* HUMIDITY_H_ */
