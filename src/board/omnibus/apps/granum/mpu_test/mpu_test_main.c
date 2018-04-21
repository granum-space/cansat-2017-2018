#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

#include <nuttx/sensors/mpu6000.h>

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int mpu_test_main(int argc, char *argv[])
#endif
{
  printf("This is MPU6000 test app!\n");
  int fd = open("/dev/mpu0", O_RDONLY | O_NONBLOCK);
  if (fd < 0)
  {
      perror("Can't open /dev/mpu0");
      return 1;
  }

  ioctl(fd, MPU6000_CMD_SET_CONVERT, true);

  mpu6000_record_t record;
  struct file * filep;

  while(1)
  {
	  fs_getfilep(fd, &filep);
      ssize_t isok = read(fd, &record, sizeof(record) );
      printf("ACC:	%0.6f	%0.6f	%0.6f\n", record.acc.x, record.acc.y, record.acc.z);
      printf("GYRO:	%0.6f	%0.6f	%0.6f\n", record.gyro.x, record.gyro.y, record.gyro.z);
      printf("TEMP: %0.6f\n", record.temperature);
      printf("TIME: %0.3f\n", (float)record.time.tv_sec + (float)record.time.tv_nsec / 1000000000 );
      printf("ERR:	%d\n", get_errno() * isok);
      printf("_________________________________________________________________\n");
      sleep(1);
  }

  return 0;
}
