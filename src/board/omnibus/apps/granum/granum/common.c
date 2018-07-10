/*
 * common.c
 *
 *  Created on: 4 июл. 2018 г.
 *      Author: developer
 */

#include <sys/types.h>

#include "common.h"

void send_telem(void * ptr, size_t len) {
	write(file_fd, ptr, len);
	fsync(file_fd);

	write(nrf_fd, ptr, len);

	write(raspberry_fd, ptr, len);
}
