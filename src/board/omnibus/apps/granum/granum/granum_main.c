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
#include <mavlink/granum/mavlink.h>
#include "../../include/gpsutils/minmea.h"
#include "madgwick/MadgwickAHRS.h"

#include "router.h"
#include "common.h"

#include "threads/madgwick_thread.h"
#include "threads/sensors_thread.h"
#include "threads/interfaces_thread.h"

int nrf_fd, file_fd, fifo_fd, raspberry_fd;

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

	char filename[16];
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

	raspberry_fd = open("/dev/ttyS2", O_RDWR | O_NONBLOCK); //FIXME уточнить номер tty



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

	fifo_fd = open("/dev/cf_fifo", O_RDWR);
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

	pthread_t sensors_thread_id;
	pthread_create(&sensors_thread_id, 0, sensors_thread, NULL);

	pthread_t interfaces_thread_id;
	pthread_create(&interfaces_thread_id, 0, interfaces_thread, NULL);

	while(true){
		//control flow here
		//reading fifo, parsing and executing fsm
	}

 	return 0;
}
