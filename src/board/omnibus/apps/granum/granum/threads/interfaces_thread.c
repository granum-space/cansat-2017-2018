/*
 * interfaces_thread.c
 *
 *  Created on: 31 мая 2018 г.
 *      Author: developer
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>

#include <nuttx/config.h>

#include <nuttx/sensors/bmp280.h>
#include <nuttx/sensors/gy_us42.h>
#include <mavlink/granum/mavlink.h>
#include "../../include/gpsutils/minmea.h"

#include "router.h"
#include "common.h"

uint8_t buffer[512];

pthread_addr_t interfaces_thread(pthread_addr_t arg)  {
	raspberry_fd = open("/dev/ttyS2", O_RDWR | O_NONBLOCK); //FIXME уточнить номер tty

	while(true) {
		int len = read(nrf_fd, buffer, 512);

		if(len > 0) {
			ROUTE(ROUTE_WAY_GROUND_COMMANDS, buffer, len)
		}

		len = read(raspberry_fd, buffer, len); //FIXME выяснить, блокируется ли

		if(len > 0) {
			ROUTE(ROUTE_WAY_TELEMETRY_RASPBERRY, buffer, len)//FIXME determine, if it is raspberry telemetry or ground commands
		}

	}
}
