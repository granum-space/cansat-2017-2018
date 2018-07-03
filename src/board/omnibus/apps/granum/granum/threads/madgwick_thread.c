/*
 * madgwick_thread.c
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

#include <nuttx/config.h>

#include <nuttx/sensors/mpu6000.h>
#include <nuttx/sensors/lsm303c.h>
#include <mavlink/granum/mavlink.h>
#include "madgwick/MadgwickAHRS.h"

#include "../common.h"

#include "./madgwick_thread.h"

#define SEC2NSEC(SEC) (SEC * 1000000000)
#define PERIOD_NSEC SEC2NSEC(1)
#define GAUSSTOT(GAUSS) (GAUSS / 10000)

#define DTORAD(DEG)	((DEG) * M_PI / 180.0f)
#define GTOMSS(G)	(G * 9.80665)

pthread_addr_t madgwick_thread(pthread_addr_t arg) {
	printf("Madgwick thread\n");

	int mpu_fd = open("/dev/mpu0", O_RDONLY | O_NONBLOCK);
	if (mpu_fd < 0)
	{
		perror("Can't open /dev/mpu0");
		return 1;
	}

	int mag_fd = open("/dev/mag0", O_RDONLY);
	if (mag_fd < 0)
	{
		perror("cant open magnitometer device");
		return 1;
	}

//Настройки MPU6000
	ioctl(mpu_fd, MPU6000_CMD_SET_CONVERT, true);
	ioctl(mpu_fd, MPU6000_CMD_SET_FILTER, 6);
	ioctl(mpu_fd, MPU6000_CMD_SET_SAMPLERATE_DIVIDER, 9);

	mpu6000_record_t record_mpu;
	lsm303c_record_t record_lsm;
	mavlink_scaled_imu_t imu_msg;

	mavlink_attitude_quaternion_t quat_msg = {
			.rollspeed = 0, .yawspeed = 0, .pitchspeed = 0
	};

	mavlink_message_t msg;
	uint8_t buffer[1024];

	struct timespec period = {
			.tv_sec = 0, .tv_nsec = SEC2NSEC(0.09)
	};

	float gyro_err_x = 0;
	float gyro_err_y = 0;
	float gyro_err_z = 0;


	for(int i = 0; i < 10; i++){
		read(mpu_fd, &record_mpu, sizeof(mpu6000_record_t) );
		clock_nanosleep(CLOCK_REALTIME, 0, &period, NULL);
		clock_nanosleep(CLOCK_REALTIME, 0, &period, NULL);
		clock_nanosleep(CLOCK_REALTIME, 0, &period, NULL);
	}

	for(int i = 0; i < 10; i++){
		read(mpu_fd, &record_mpu, sizeof(mpu6000_record_t) );
		gyro_err_x += record_mpu.gyro.x;
		gyro_err_y += record_mpu.gyro.y;
		gyro_err_z += record_mpu.gyro.z;
		clock_nanosleep(CLOCK_REALTIME, 0, &period, NULL);
		clock_nanosleep(CLOCK_REALTIME, 0, &period, NULL);
		clock_nanosleep(CLOCK_REALTIME, 0, &period, NULL);
	}

	gyro_err_x /= 10.0f;
	gyro_err_y /= 10.0f;
	gyro_err_z /= 10.0f;

	timer_t timer;
	struct sigevent evt = {
		.sigev_notify = SIGEV_SIGNAL,
		.sigev_signo = SIGALRM,
		.sigev_value = 0,
	};
	volatile int ret = timer_create(CLOCK_REALTIME, &evt, &timer);

	struct itimerspec timerspec = {
		.it_value = period,
		.it_interval = period,
	};
	ret = timer_settime(timer, 0, &timerspec, NULL);

	sigset_t waitset = (sigset_t)0;
	sigaddset(&waitset, SIGALRM);

	ioctl(mpu_fd, MPU6000_CMD_FLUSHFIFO, 0);
	while(true) {
		sigsuspend(&waitset);

		ssize_t isok = read(mpu_fd, &record_mpu, sizeof(record_mpu));
		DEBUG("MPU6000: got %d bytes, error %d\n", isok >= 0 ? isok : 0, isok >=0 ? 0 : -get_errno());
		if(isok < 0) continue;

		isok = read(mag_fd, &record_lsm, sizeof(record_lsm));
		DEBUG("LSM303C: got %d bytes, error %d\n", isok >= 0 ? isok : 0, isok >=0 ? 0 : -get_errno());
		if(isok < 0) continue;

		MadgwickAHRSupdate(DTORAD(record_mpu.gyro.x - gyro_err_x), \
				DTORAD(record_mpu.gyro.z - gyro_err_z), \
				DTORAD(record_mpu.gyro.y - gyro_err_y), \
				GTOMSS(record_mpu.acc.x), GTOMSS(record_mpu.acc.z), GTOMSS(record_mpu.acc.y), \
				GAUSSTOT(-record_lsm.field.y), GAUSSTOT(-record_lsm.field.z), GAUSSTOT(-record_lsm.field.x) ); //Remapped according to real sensors attitude

		static int repetitions = 0;
		if(repetitions < 100) {
			repetitions++;
			if(repetitions == 100) beta = 0.041;
		}

		static int ticker = 0;
		ticker++;
		if(ticker == 5) {
			ticker = 0;

			imu_msg.time_boot_ms = record_mpu.time.tv_sec * 1000 + record_mpu.time.tv_nsec / 1000000;
			imu_msg.xacc = (int)(record_mpu.acc.x * 1000.0f); //convert to mG
			imu_msg.yacc = (int)(record_mpu.acc.y * 1000.0f);
			imu_msg.zacc = (int)(record_mpu.acc.z * 1000.0f);
			imu_msg.xgyro = (int)(DTORAD((record_mpu.gyro.x - gyro_err_x)) * 1000.0f); //convert to mDPS
			imu_msg.ygyro = (int)(DTORAD((record_mpu.gyro.y - gyro_err_y)) * 1000.0f);
			imu_msg.zgyro = (int)(DTORAD((record_mpu.gyro.z - gyro_err_z)) * 1000.0f);
			imu_msg.xmag = (int)(record_lsm.field.x / 10.0f); //convert to mT
			imu_msg.ymag = (int)(record_lsm.field.y / 10.0f);
			imu_msg.zmag = (int)(record_lsm.field.z / 10.0f);

			mavlink_msg_scaled_imu_encode(GR_SYSTEM_OMNIBUS, GR_COMPONENT_OMNIBUS_MADJWICK, &msg, &imu_msg);
			uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);

			DEBUG("Madgwick 1\n");

			ROUTE(ROUTE_WAY_TELEMETRY_COMMON, buffer, len)

			quat_msg.q1 = q0;
			quat_msg.q2 = q1;
			quat_msg.q3 = q2;
			quat_msg.q4 = q3;

			mavlink_msg_attitude_quaternion_encode(GR_SYSTEM_OMNIBUS, GR_COMPONENT_OMNIBUS_MADJWICK, &msg, &quat_msg);
			len = mavlink_msg_to_send_buffer(buffer, &msg);

			DEBUG("Madgwick 2\n");

			ROUTE(ROUTE_WAY_TELEMETRY_COMMON, buffer, len)
		}
	}

	return NULL;
}
