#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int blinky_main(int argc, char *argv[])
#endif
{
  printf("hello world, this is blinky app!\n");
  int fd = open("/dev/range0", O_RDWR);
  if (fd < 0)
  {
      perror("cant open sonar device");
      return 1;
  }

  uint16_t val;
  int rc = read(fd, &val, 2);
  printf("rc: %d,    val: %d", rc, val);
  return 0;
}
