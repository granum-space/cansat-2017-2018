/*
 * sensors_thread.c
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


pthread_addr_t sensors_thread(pthread_addr_t arg) {

	while(1)
	{

	}
}
