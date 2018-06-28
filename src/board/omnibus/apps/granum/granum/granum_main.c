#include <sys/stat.h>
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
#include <nuttx/sensors/bmp280.h>
#include <nuttx/sensors/gy_us42.h>
#include <nuttx/wireless/nrf24l01.h>
#include <nuttx/sensors/tsl2561.h>

#include <mavlink/granum/mavlink.h>
#include "../../include/gpsutils/minmea.h"
#include "madgwick/MadgwickAHRS.h"

#include "router.h"
#include "common.h"

#include "threads/madgwick_thread.h"
#include "threads/sensors_thread.h"
#include "threads/interfaces_thread.h"

//Global variables
int nrf_fd, file_fd, fifo_fd, raspberry_fd;
gr_state_t gr_state = GR_STATE_BOOTING;

//Settings
#define GR_CONFIG_LUX_THRESHOLD 400
#define GR_CONFIG_DISTANCE_FREE_FALL_THRESHOLD 50


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

static void _translate_state(bool open_legs) {
	switch(gr_state) {
	case GR_STATE_BOOTING:
	case GR_STATE_READY:
		gr_state++;
		break;

	case GR_STATE_LOADED:
		//FIXME starting spectro observation
		gr_state++;
		break;

	case GR_STATE_FREE_FALL:
		//FIXME open chute
		gr_state++;
		break;

	case GR_STATE_AWAITING_CHUTE:
		if(open_legs) {
			//FIXME open legs
		}

		gr_state++;
		break;

	case GR_STATE_PRE_LANDING:
		//FIXME detach chute
		gr_state++;
		break;

	default:
		break; //May be we should report?

	}
}

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int granum_main(int argc, char *argv[])
#endif
{
	printf("Starting main mission!\n");
	sleep(3);
	printf("End of sleep\n");

//Opening nrf and file
	nrf_fd = open("/dev/nrf24l01", O_RDWR | O_NONBLOCK);
	if (nrf_fd < 0)
	{
	  perror("Can't open /dev/nrf24l01");
	  return 1;
	}

	char filename[32];
	struct stat stats;

	for(int i = 0; i < 100; i++) {
		snprintf(filename, 22, "/mnt/sd0/GRANUM%d.bin", i);
		int ret = stat(filename, &stats);

		if(ret == -1 && get_errno() == ENOENT) {
			file_fd = open(filename, O_WRONLY | O_CREAT);
			break;
		}

		if(i == 99) {
			perror("There are 99 telemetry files already");
			return 1;
		}
	}

	if (file_fd < 0)
	{
		perror("Can't open telemetry file");
		perror(filename);
		return 1;
	}

	raspberry_fd = open("/dev/ttyS1", O_RDWR | O_NONBLOCK);



//Настройки NRF24L01+
	uint32_t tmp = 2489;
	ioctl(nrf_fd, WLIOC_SETRADIOFREQ, (long unsigned int)&tmp);

	tmp = 5;
	ioctl(nrf_fd, NRF24L01IOC_SETADDRWIDTH, (long unsigned int)&tmp);

	uint8_t addr[] = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB};
	ioctl(nrf_fd, NRF24L01IOC_SETTXADDR, (long unsigned int)addr);

	tmp = 0;
	ioctl(nrf_fd, WLIOC_SETTXPOWER, (long unsigned int)&tmp);

	nrf24l01_retrcfg_t retrcfg = {.delay = DELAY_250us, .count = 4};
	ioctl(nrf_fd, NRF24L01IOC_SETRETRCFG, (long unsigned int)&retrcfg);

	nrf24l01_pipecfg_t pipe0 = {.en_aa = true,\
							  .payload_length = NRF24L01_DYN_LENGTH};
	pipe0.rx_addr[0] = 0xAA;
	pipe0.rx_addr[1] = 0xAA;
	pipe0.rx_addr[2] = 0xAA;
	pipe0.rx_addr[3] = 0xAA;
	pipe0.rx_addr[4] = 0xAA;

	nrf24l01_pipecfg_t * pipes[] = {&pipe0, (nrf24l01_pipecfg_t *)NULL, (nrf24l01_pipecfg_t *)NULL, (nrf24l01_pipecfg_t *)NULL, (nrf24l01_pipecfg_t *)NULL, (nrf24l01_pipecfg_t *)NULL};

	ioctl(nrf_fd, NRF24L01IOC_SETPIPESCFG, (long unsigned int)pipes);

	uint8_t pipecfg = 0x01;
	ioctl(nrf_fd, NRF24L01IOC_SETPIPESENABLED, (long unsigned int)&pipecfg);

	nrf24l01_datarate_t datarate = RATE_1Mbps;
	ioctl(nrf_fd, NRF24L01IOC_SETDATARATE, (long unsigned int)&datarate);

	nrf24l01_crcmode_t crcmode = CRC_2BYTE;
	ioctl(nrf_fd, NRF24L01IOC_SETCRCMODE, (long unsigned int)&crcmode);

	nrf24l01_state_t state = ST_STANDBY;
	ioctl(nrf_fd, NRF24L01IOC_SETSTATE, (long unsigned int)&state);



//Create fifo and open reading end
	int ret = mkfifo("/dev/cf_fifo", 0666);
	if(ret != 0) {
		perror("Can't create fifo");
		return 1;
	}

	fifo_fd = open("/dev/cf_fifo", O_RDWR | O_NONBLOCK);
	if (fifo_fd < 0)
	{
		perror("Can't open telemetry file");
		perror(filename);
		return 1;
	}


//Messages instances
	mavlink_message_t msg;
	uint8_t buffer[1024];



//Running threads
	pthread_t madgwick_thread_id;
	pthread_create(&madgwick_thread_id, 0, madgwick_thread, NULL);

	pthread_t interfaces_thread_id;
	pthread_create(&interfaces_thread_id, 0, interfaces_thread, NULL);

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

	int tsl_fd = open("/dev/lumen0", O_RDONLY);
	if (tsl_fd < 0)
	{
	  perror("cant open lumen device");
	  return 1;
	}

	ioctl(tsl_fd, TSL2561_IOCTL_CMD_SETUP, NULL);


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
	mavlink_luminosity_t luminosity_msg;

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

	mavlink_granum_message_t msg_msg;

	mavlink_message_t msg;
	uint8_t buffer[1024];

	gr_state++;

	struct timespec current_time;

	while(true){
		clock_gettime(CLOCK_MONOTONIC, &current_time);
		uint64_t time_boot_ms = current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000;

//TSL2561
		read(tsl_fd, &luminosity_msg.luminosity, 2);
		luminosity_msg.time_boot_ms = time_boot_ms;
		mavlink_msg_luminosity_encode(GR_SYSTEM_OMNIBUS, GR_COMPONENT_OMNIBUS_MAIN, &msg, &sonar_msg);

		int len = mavlink_msg_to_send_buffer(buffer, &msg);

		ROUTE(ROUTE_WAY_TELEMETRY_COMMON, buffer, len)

		if(gr_state == GR_STATE_LOADED && luminosity_msg.luminosity >= GR_CONFIG_LUX_THRESHOLD) _translate_state(NULL);

//BMP280
		ssize_t isok = read(baro_fd, &result, sizeof(result) );
		DEBUG("BMP280: got %d bytes, error %d\n", isok >= 0 ? isok : 0, isok >=0 ? 0 : -get_errno());

		clock_gettime(CLOCK_MONOTONIC, &current_time);
		baro_msg.time_boot_ms = current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000;
		baro_msg.press_abs = result.pressure / 100.0;				//pressure in hPa
		baro_msg.temperature = result.temperature * 100.0;		//temperature in 0.01 degC

		mavlink_msg_scaled_pressure_encode(0, MAV_COMP_ID_PERIPHERAL, &msg, &baro_msg);
		uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);

		ROUTE(ROUTE_WAY_TELEMETRY_COMMON, buffer, len)

		if(gr_state == GR_STATE_FREE_FALL || gr_state == GR_STATE_AWAITING_CHUTE) {
			//TODO logic
		}

//SONAR
		read(sonar_fd, &sonar_msg.distance, 2);
		ioctl(sonar_fd, GY_US42_IOCTL_CMD_MEASURE, (unsigned int)NULL);
		sonar_msg.time_boot_ms = time_boot_ms;
		mavlink_msg_sonar_encode(GR_SYSTEM_OMNIBUS, GR_COMPONENT_OMNIBUS_MAIN, &msg, &sonar_msg);

		len = mavlink_msg_to_send_buffer(buffer, &msg);

		ROUTE(ROUTE_WAY_TELEMETRY_COMMON, buffer, len)

		if(gr_state == GR_STATE_LOADED && sonar_msg.distance > GR_CONFIG_DISTANCE_FREE_FALL_THRESHOLD) _translate_state(NULL);

		if(gr_state == GR_STATE_PRE_LANDING) {

		}

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

			ROUTE(ROUTE_WAY_TELEMETRY_COMMON, buffer, len)
		}

		usleep(100000);
	}

	printf("Something not good happened, and so, we are leaving main thread, brothers!\n");

 	return 0;
}
