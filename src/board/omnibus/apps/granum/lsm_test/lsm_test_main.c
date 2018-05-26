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

#include <nuttx/sensors/lsm303c.h>

#define DEBUG printf

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int lsm_test_main(int argc, char *argv[])
#endif
{
	int fd = open("/dev/mag0", O_RDONLY | O_NONBLOCK);
	if (fd < 0)
	{
		perror("Can't open /dev/mag0");
		return 1;
	}

	lsm303c_record_t record;

	while(1)
	{
//LSM303C
		ssize_t isok = read(fd, &record, sizeof(record) );
		DEBUG("LSM303C: got %d bytes, error %d\n", isok >= 0 ? isok : 0, isok >=0 ? 0 : -get_errno());
		DEBUG("%0.6f %0.6f %0.6f\n", record.field.x, record.field.y, record.field.z);
		usleep(500000);
	}

 	return 0;
}
