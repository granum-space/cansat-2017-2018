#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "../../include/gpsutils/minmea.h"

#define MSG_BUF_SIZE 256

static size_t _msg_carret;
static char _msg_buffer[MSG_BUF_SIZE];

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int gps_test_main(int argc, char *argv[])
#endif
{
	printf("This is GPS minmea test app!\n");

	int fd = open("/dev/ttyS1", O_RDONLY);

	if (fd < 0)
	{
		perror("Can't open /dev/ttyS1");
		return 1;
	}

	struct termios termios;

	int ret = ioctl(fd, TCGETS, (unsigned long) &termios);

	termios.c_cflag = 	(CS8 	 	//8 bits
						| CREAD) 	//Enable read
						& ~CSTOPB	//1 stop bit
						& ~PARENB	//Disable parity check
						& ~CRTSCTS; //No hardware flow control

	ret = ioctl(fd, TCSETS, (unsigned long) &termios);

	char string[256];
	while(1) {
		/*ssize_t ret = read(fd, string, 1);
		size_t len = ret < 0 ? 0 : ret;
		int err = ret < 0 ? -get_errno() : 0;
		string[len] = 0;

		printf(string);*/

		// ждем доллара
		do {
			ret = read(fd, _msg_buffer, 1);
			if(ret < 0) continue;
		} while (_msg_buffer[0] != '$');

		// Оп! нашли
		_msg_carret = 1;

		// теперь накапливаем все до \r\n
		do {
			read(fd, _msg_buffer + (_msg_carret++), 1);
			if (_msg_carret >= MSG_BUF_SIZE)
			{
				// что-то не так
				printf("Buffer overflow error!\n");
				break;
			}

		} while('\r' != _msg_buffer[_msg_carret-2]
			||	'\n' != _msg_buffer[_msg_carret-1]
		);

		// в конце терминируем строку
		_msg_buffer[_msg_carret] = '\x00';

		// накопили, теперь разбираем
		if (!minmea_check(_msg_buffer, false))
			continue;

		struct minmea_sentence_gga frame;
		if (!minmea_parse_gga(&frame, _msg_buffer))
			continue; // опс, что-то пошло не так

		if (frame.fix_quality == 0)
			continue;

		float _lon = minmea_tofloat(&frame.longitude);
		float _lat = minmea_tofloat(&frame.latitude);
		float _height = minmea_tofloat(&frame.altitude);
		bool _hasFix = frame.fix_quality != 0;

		printf("LON %f LAT %f HEI %f HASFIX %s\n", _lon, _lat, _height, _hasFix ? "TRUE" : "FALSE");
	}

	return 0;
}
