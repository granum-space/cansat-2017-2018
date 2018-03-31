#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <nuttx/sensors/gy_us42.h>

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int sonar_main(int argc, char *argv[])
#endif
{
  printf("hello world, this is sonar app!\n");
  int fd = open("/dev/range0", O_RDONLY);
  if (fd < 0)
  {
      perror("cant open sonar device");
      return 1;
  }

  uint16_t val;
  unsigned long arg;
  int rc1, rc2;
  int cmd = GY_US42_IOCTL_CMD_MEASURE;
  while(1) {
	  rc1 = ioctl(fd, cmd, arg);
	  usleep(500000);
	  rc2 = read(fd, &val, 2);
	  printf("rc1: %d, rc2 %d, val: %d, 0x%04X\n", rc1, rc2, val, val);
	  sleep(1);
  }
  return 0;
}
