#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
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

  uint16_t lux = 0;
  unsigned long arg;
  ioctl(fd, TSL2561_IOCTL_CMD_SETUP, arg);
  while(1) {
	  read(fd, &lux, 2);
	  usleep(5000);
	  printf("lux: %d\n", lux);
	  sleep(2);
  }
  return 0;
}
