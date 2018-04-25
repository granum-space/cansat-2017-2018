#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

#include <nuttx/sensors/mpu6000.h>
#include <nuttx/wireless/nrf24l01.h>
#include <mavlink/granum/mavlink.h>

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

	mpu6000_record_t record;
	struct file * filep;

	mavlink_scaled_mpu6000_t mpu_msg;
	mavlink_message_t msg;

	uint8_t buffer[1024];

	while(1)
	{
		fs_getfilep(mpu_fd, &filep);
		ssize_t isok = read(mpu_fd, &record, sizeof(record) );
		printf("MPU6000: got %d bytes, error %d\n", isok >= 0 ? isok : 0, isok >=0 ? 0 : -get_errno());
		if(isok < 0) continue;

		mpu_msg.time_boot_ms = record.time.tv_sec * 1000 + record.time.tv_nsec / 1000000;
		mpu_msg.xacc = (int)(record.acc.x * 1000);
		mpu_msg.yacc = (int)(record.acc.y * 1000);
		mpu_msg.zacc = (int)(record.acc.z * 1000);
		mpu_msg.xgyro = (int)(record.gyro.x * 1000 * M_PI / 180);
		mpu_msg.ygyro = (int)(record.gyro.y * 1000 * M_PI / 180);
		mpu_msg.zgyro = (int)(record.gyro.z * 1000 * M_PI / 180);
		mpu_msg.temperature = (int)(record.temperature * 1000);

		mavlink_msg_scaled_mpu6000_encode(0, MAV_COMP_ID_IMU, &msg, &mpu_msg);
		uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);

		isok = write(nrf_fd, buffer, len);
		printf("NRF: wrote %d bytes, error %d\n", isok >= 0 ? isok : 0, isok >=0 ? 0 : -get_errno());

		printf("_________________________________________________________________\n");
		sleep(1);
	}

 	return 0;
}
