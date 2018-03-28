#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int sonar_main(int argc, char *argv[])
#endif
{
  printf("hello world, this is blinky app!\n");
  int fd = open("/dev/userleds", O_WRONLY);
  if (fd < 0)
  {
      perror("cant open led device");
      return 1;
  }


  uint8_t values[] = {
          (0 << 0),
          (1 << 0)
  };
  int ptr = 0;

  while(1)
  {
      write(fd, &values[ptr], 1);
      printf("writed %d\n", (int)values[ptr]);
      usleep(500000);
      ptr++;
      if (ptr >= sizeof(values))
          ptr = 0;
  }

  return 0;
}
