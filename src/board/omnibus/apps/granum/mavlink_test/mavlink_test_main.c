#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>

#include <nuttx/config.h>

#include <nuttx/sensors/mpu6000.h>
#include <nuttx/sensors/bmp280.h>
#include <nuttx/sensors/gy_us42.h>
#include <nuttx/wireless/nrf24l01.h>
#include <mavlink/granum/mavlink.h>
#include "../../include/gpsutils/minmea.h"

#define MSG_BUF_SIZE 256

static size_t _msg_carret = 0;
static char _msg_buffer[MSG_BUF_SIZE];

bool parseGPS(int fd, int cycles) {
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

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int mavlink_test_main(int argc, char *argv[])
#endif
{
	printf("This is MAVLink test app!\n");
	int mpu_fd = open("/dev/mpu0", O_RDONLY | O_NONBLOCK);
	if (mpu_fd < 0)
	{
		perror("Can't open /dev/mpu0");
		return 1;
	}

	int nrf_fd = open("/dev/nrf24l01", O_RDWR | O_NONBLOCK);
	if (nrf_fd < 0)
	{
	  perror("Can't open /dev/nrf24l01");
	  return 1;
	}

	int baro_fd = open("/dev/baro0", O_RDWR | O_NONBLOCK);
	if (baro_fd < 0)
	{
	  perror("Can't open /dev/baro0");
	  return 1;
	}

	int gps_fd = open("/dev/ttyS1", O_RDONLY);

	if (gps_fd < 0)
	{
		perror("Can't open /dev/ttyS1");
		return 1;
	}

	int fd = open("/dev/sonar0", O_RDONLY);
	if (fd < 0)
	{
	  perror("cant open sonar device");
	  return 1;
	}

	ioctl(mpu_fd, MPU6000_CMD_SET_CONVERT, true);

	uint32_t tmp = 2489;
	ioctl(nrf_fd, WLIOC_SETRADIOFREQ, (long unsigned int)&tmp);

	tmp = 5;
	ioctl(nrf_fd, NRF24L01IOC_SETADDRWIDTH, (long unsigned int)&tmp);

	uint8_t addr[] = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB};
	ioctl(nrf_fd, NRF24L01IOC_SETTXADDR, (long unsigned int)addr);

	tmp = 0;
	ioctl(nrf_fd, WLIOC_SETTXPOWER, (long unsigned int)&tmp);

	nrf24l01_retrcfg_t retrcfg = {.delay = DELAY_4000us, .count = 15};
	ioctl(nrf_fd, NRF24L01IOC_SETRETRCFG, &retrcfg);

	nrf24l01_pipecfg_t pipe0 = {.en_aa = true,\
							  .payload_length = NRF24L01_DYN_LENGTH};
	pipe0.rx_addr[0] = 0xAA;
	pipe0.rx_addr[1] = 0xAA;
	pipe0.rx_addr[2] = 0xAA;
	pipe0.rx_addr[3] = 0xAA;
	pipe0.rx_addr[4] = 0xAA;

	nrf24l01_pipecfg_t pipeDummy = {.en_aa = true,\
							  .payload_length = NRF24L01_DYN_LENGTH};
	pipeDummy.rx_addr[0] = 0xAA;
	pipeDummy.rx_addr[1] = 0xAA;
	pipeDummy.rx_addr[2] = 0xAA;
	pipeDummy.rx_addr[3] = 0xAA;
	pipeDummy.rx_addr[4] = 0xAA;

	nrf24l01_pipecfg_t * pipes[] = {&pipe0, &pipeDummy, &pipeDummy, &pipeDummy, &pipeDummy, &pipeDummy};

	ioctl(nrf_fd, NRF24L01IOC_SETPIPESCFG, (long unsigned int)pipes);

	uint8_t pipecfg = 0x01;
	ioctl(nrf_fd, NRF24L01IOC_SETPIPESENABLED, (long unsigned int)&pipecfg);

	nrf24l01_datarate_t datarate = RATE_1Mbps;
	ioctl(nrf_fd, NRF24L01IOC_SETDATARATE, (long unsigned int)&datarate);

	nrf24l01_crcmode_t crcmode = CRC_2BYTE;
	ioctl(nrf_fd, NRF24L01IOC_SETCRCMODE, (long unsigned int)&crcmode);

	nrf24l01_state_t state = ST_STANDBY;
	ioctl(nrf_fd, NRF24L01IOC_SETSTATE, &state);

	struct termios termios;

	ioctl(gps_fd, TCGETS, (unsigned long) &termios);

	termios.c_cflag = 	(CS8 	 	//8 bits
						| CREAD) 	//Enable read
						& ~CSTOPB	//1 stop bit
						& ~PARENB	//Disable parity check
						& ~CRTSCTS; //No hardware flow control

	ioctl(gps_fd, TCSETS, (unsigned long) &termios);

	mpu6000_record_t record;
	struct file * filep;

	mavlink_scaled_imu_t imu_msg;
	mavlink_scaled_pressure_t baro_msg;
	mavlink_sonar_t sonar_msg;
	mavlink_hil_gps_t gps_msg;
	mavlink_message_t msg;

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

	bmp280_data_t result = {0, 0};

	uint8_t buffer[1024];

	while(1)
	{
		fs_getfilep(mpu_fd, &filep);
		ssize_t isok = read(mpu_fd, &record, sizeof(record) );
		printf("MPU6000: got %d bytes, error %d\n", isok >= 0 ? isok : 0, isok >=0 ? 0 : -get_errno());

		imu_msg.time_boot_ms = record.time.tv_sec * 1000 + record.time.tv_nsec / 1000000;
		imu_msg.xacc = (int)(record.acc.x * 1000);
		imu_msg.yacc = (int)(record.acc.y * 1000);
		imu_msg.zacc = (int)(record.acc.z * 1000);
		imu_msg.xgyro = (int)(record.gyro.x * 1000 * M_PI / 180);
		imu_msg.ygyro = (int)(record.gyro.y * 1000 * M_PI / 180);
		imu_msg.zgyro = (int)(record.gyro.z * 1000 * M_PI / 180);
		imu_msg.xmag = 0x7fff;
		imu_msg.ymag = 0x7fff;
		imu_msg.zmag = 0x7fff;

		mavlink_msg_scaled_imu_encode(0, MAV_COMP_ID_IMU, &msg, &imu_msg);
		uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);

		isok = write(nrf_fd, buffer, len);
		printf("NRF: wrote %d bytes, error %d\n", isok >= 0 ? isok : 0, isok >=0 ? 0 : -get_errno());

		printf("_________________________________________________________________\n");



		isok = read(baro_fd, &result, sizeof(result) );
		printf("BMP280: got %d bytes, error %d\n", isok >= 0 ? isok : 0, isok >=0 ? 0 : -get_errno());

		struct timespec current_time;
		clock_gettime(CLOCK_MONOTONIC, &current_time);
		baro_msg.time_boot_ms = current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000;
		baro_msg.press_abs = result.pressure / 100.0;				//pressure in hPa
		baro_msg.temperature = result.temperature * 100.0;		//temperature in 0.01 degC

		mavlink_msg_scaled_pressure_encode(0, MAV_COMP_ID_PERIPHERAL, &msg, &baro_msg);

		len = mavlink_msg_to_send_buffer(buffer, &msg);

		isok = write(nrf_fd, buffer, len);
		printf("NRF: wrote %d bytes, error %d\n", isok >= 0 ? isok : 0, isok >=0 ? 0 : -get_errno());
		printf("_________________________________________________________________\n");


		read(fd, &sonar_msg.distance, 2);
		clock_gettime(CLOCK_MONOTONIC, &current_time);
		ioctl(fd, GY_US42_IOCTL_CMD_MEASURE, (unsigned int)NULL);
		sonar_msg.time_boot_ms = current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000;
		mavlink_msg_sonar_encode(0, MAV_COMP_ID_PERIPHERAL, &msg, &sonar_msg);

		len = mavlink_msg_to_send_buffer(buffer, &msg);

		isok = write(nrf_fd, buffer, len);
		printf("NRF: wrote %d bytes, error %d\n", isok >= 0 ? isok : 0, isok >=0 ? 0 : -get_errno());
		printf("_________________________________________________________________\n");



		if( parseGPS(gps_fd, 100) ) {
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

			isok = write(nrf_fd, buffer, len);
			printf("NRF: wrote %d bytes, error %d\n", isok >= 0 ? isok : 0, isok >=0 ? 0 : -get_errno());
			printf("_________________________________________________________________\n");
		}



		sleep(1);
	}

 	return 0;
}
