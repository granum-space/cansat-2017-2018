#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <nuttx/sensors/tsl2561.h>

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int lumen_main(int argc, char *argv[])
#endif
{
  printf("hello world, this is lumen app!\n");
  int fd = open("/dev/lumen0", O_RDONLY);
  if (fd < 0)
  {
      perror("cant open lumen device");
      return 1;
  }

  unsigned int lux;
  unsigned long arg;
  int ioctl1, ioctl2;
  int read1;
  uint8_t* ret;
  while(1) {
	  read1 = read(fd, ret, 2);
	  ioctl1 = ioctl(fd, TSL2561_IOCTL_CMD_MEASURE_CHANNEL_0, arg);
	  ioctl2 = ioctl(fd, TSL2561_IOCTL_CMD_MEASURE_CHANNEL_1, arg);
	  lux = ioctl(fd, TSL2561_IOCTL_CMD_GETLUX, arg);
	  usleep(5000);
	  printf("lux: %d % d %d\n", ioctl1, ioctl2, lux);
	  sleep(2);
  }
  return 0;
}
