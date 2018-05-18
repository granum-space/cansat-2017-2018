/*
 * humidity.c
 *
 *  Created on: 11 мая 2018 г.
 *      Author: developer
 */
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "humidity.h"

#define I2C_DEVICE "/dev/i2c-1"
#define AM2320_ADDR 0x5C

static uint16_t _unite_bytes(uint8_t msb, uint8_t lsb)
{
	return ((uint16_t)msb << 8) | (uint16_t)lsb;
}

am2320_errors_t am2320_get_data(float* humidity, float* temperature)
{
	int fd;
	int8_t buf[8];

	fd = open(I2C_DEVICE, O_RDWR);
	if (fd < 0) {
		return AM2320_ERROR_OPEN;
	}

	uint8_t addr = AM2320_ADDR;
	if (ioctl(fd, I2C_SLAVE, addr) < 0) {
		perror("error in ioctl: ");
		return AM2320_ERRROR_IOCTL;
	}

	write(fd, NULL, 0);
	usleep(1000); /* at least 0.8ms, at most 3ms */

	buf[0] = 0x03; /* read function code */
	buf[1] = 0x00; /* start address */
	buf[2] = 0x04; /* number of registers to read */
	if (write(fd, buf, 3) < 0) {
		perror("error 3: ");
		return AM2320_ERROR_WRITE;
	}
	usleep(1600); /* wait at least 1.5ms */
	/*
	 * 0   byte: function code
	 * 1   byte: number of bytes
	 * 2   byte: humidity high byte
	 * 3   byte: humidity low byte
	 * 4   byte: temperature high byte
	 * 5   byte: temperature low byte
	 * 6-7 byte: CRC
	 */
	if (read(fd, buf, 8) < 0) {
		perror("error in reading: ");
		return AM2320_ERROR_READ;
	}

	int16_t h = _unite_bytes(buf[2], buf[3]);
	int16_t t = _unite_bytes(buf[4], buf[5]);

	*humidity = (float)h / 10.0;
	*temperature = (float)t / 10.0;

	return AM2320_ERROR_OK;
}
