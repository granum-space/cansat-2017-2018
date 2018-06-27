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

#define MSG_BUF_SIZE 256

static size_t _msg_carret = 0;
static char _msg_buffer[MSG_BUF_SIZE];

static bool _parseGPS(int fd, int cycles) {
	while(cycles--) {
		if(!_msg_carret) {
			read(fd, _msg_buffer, 1);
			if(_msg_buffer[0] == '$') _msg_carret++;
		}
		else {
			read(fd, _msg_buffer + (_msg_carret++), 1);

			if (_msg_carret >= MSG_BUF_SIZE)
			{
				// что-то не так
				printf("Buffer overflow error!\n");
				_msg_carret = 0;
				break;
			}

			if('\r' == _msg_buffer[_msg_carret-2]
			   &&	'\n' == _msg_buffer[_msg_carret-1]) {
				_msg_buffer[_msg_carret] = '\x00';
				_msg_carret = 0;
				return true;
			}
		}
	}

	return false;
}

pthread_addr_t sensors_thread(pthread_addr_t arg) {
	printf("Sensors thread\n");

//Opening files
	int baro_fd = open("/dev/baro0", O_RDWR | O_NONBLOCK);
	if (baro_fd < 0)
	{
		perror("Can't open /dev/baro0");
		return 1;
	}

	int gps_fd = open("/dev/ttyS2", O_RDONLY);

	if (gps_fd < 0)
	{
		perror("Can't open /dev/ttyS2");
		return 1;
	}

	int sonar_fd = open("/dev/sonar0", O_RDONLY);
	if (sonar_fd < 0)
	{
		perror("cant open sonar device");
		return 1;
	}



//Settings
//Настройки UART4 для GPS
	struct termios termios;

	ioctl(gps_fd, TCGETS, (unsigned long) &termios);

	termios.c_cflag = 	(CS8 	 	//8 bits
						| CREAD) 	//Enable read
						& ~CSTOPB	//1 stop bit
						& ~PARENB	//Disable parity check
						& ~CRTSCTS; //No hardware flow control

	ioctl(gps_fd, TCSETS, (unsigned long) &termios);



//Messages and records
	mavlink_scaled_pressure_t baro_msg;
	bmp280_data_t result = {0, 0};

	mavlink_sonar_t sonar_msg;

	mavlink_hil_gps_t gps_msg;
	gps_msg.cog = 0;
	gps_msg.eph = 0;
	gps_msg.epv = 0;
	gps_msg.vd = 0;
	gps_msg.ve = 0;
	gps_msg.vel = 0;
	gps_msg.vn = 0;
	gps_msg.satellites_visible = 0xff;
	struct timespec gps_time;
	struct minmea_date gps_basedate = {
			.year = 18,
			.month = 5,
			.day = 1,
	};

	mavlink_message_t msg;
	uint8_t buffer[1024];



	while(1)
	{
//BMP280
		ssize_t isok = read(baro_fd, &result, sizeof(result) );
		DEBUG("BMP280: got %d bytes, error %d\n", isok >= 0 ? isok : 0, isok >=0 ? 0 : -get_errno());

		struct timespec current_time;
		clock_gettime(CLOCK_MONOTONIC, &current_time);
		baro_msg.time_boot_ms = current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000;
		baro_msg.press_abs = result.pressure / 100.0;				//pressure in hPa
		baro_msg.temperature = result.temperature * 100.0;		//temperature in 0.01 degC

		mavlink_msg_scaled_pressure_encode(0, MAV_COMP_ID_PERIPHERAL, &msg, &baro_msg);
		uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);

		DEBUG("Sensor 1\n");

		ROUTE(ROUTE_WAY_TELEMETRY_COMMON, buffer, len)

//SONAR
		read(sonar_fd, &sonar_msg.distance, 2);
		clock_gettime(CLOCK_MONOTONIC, &current_time);
		ioctl(sonar_fd, GY_US42_IOCTL_CMD_MEASURE, (unsigned int)NULL);
		sonar_msg.time_boot_ms = current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000;
		mavlink_msg_sonar_encode(0, MAV_COMP_ID_PERIPHERAL, &msg, &sonar_msg);

		len = mavlink_msg_to_send_buffer(buffer, &msg);

		DEBUG("Sensor 2\n");

		ROUTE(ROUTE_WAY_TELEMETRY_COMMON, buffer, len)

//GPS
		if( _parseGPS(gps_fd, 100) ) {
			// накопили, теперь разбираем
			if (!minmea_check(_msg_buffer, false))
				continue;

			struct minmea_sentence_gga frame;
			if (!minmea_parse_gga(&frame, _msg_buffer))
				continue; // опс, что-то пошло не так

			if (frame.fix_quality == 0)
				continue;

			minmea_gettime(&gps_time, &gps_basedate, &frame.time);

			gps_msg.time_usec = gps_time.tv_sec * 1000000.0 + gps_time.tv_nsec / 1000.0;
			gps_msg.lon = (int32_t)(minmea_tocoord(&frame.longitude) * 10000000);
			gps_msg.lat = (int32_t)(minmea_tocoord(&frame.latitude) * 10000000);
			gps_msg.alt = (int32_t)(minmea_tofloat(&frame.altitude) * 1000);
			gps_msg.fix_type = frame.fix_quality;

			mavlink_msg_hil_gps_encode(0, MAV_COMP_ID_PERIPHERAL, &msg, &gps_msg);
			len = mavlink_msg_to_send_buffer(buffer, &msg);

			DEBUG("Sensor 3\n");

			ROUTE(ROUTE_WAY_TELEMETRY_COMMON, buffer, len)
		}

		usleep(100000);
	}
}
