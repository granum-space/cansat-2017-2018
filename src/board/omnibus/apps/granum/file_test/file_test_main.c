#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int file_test_main(int argc, char *argv[])
#endif
{
	printf("Hello world, this is file test!\n");
	int fd = open("/mnt/sd0/test/1", O_WRONLY|O_CREAT);
	if (fd < 0)
	{
		perror("cant open 1 file");
		return 1;
	}

	fd = open("/mnt/sd0/test/2", O_WRONLY|O_CREAT);
	if (fd < 0)
	{
		perror("cant open 2 file");
		return 1;
	}

	fd = open("/mnt/sd0/test/3", O_WRONLY|O_CREAT);
	if (fd < 0)
	{
		perror("cant open 3 file");
		return 1;
	}

	fd = open("/mnt/sd0/test/4", O_WRONLY|O_CREAT);
	if (fd < 0)
	{
		perror("cant open 4 file");
		return 1;
	}

	fd = open("/mnt/sd0/test/5", O_WRONLY|O_CREAT);
	if (fd < 0)
	{
		perror("cant open 5 file");
		return 1;
	}

	fd = open("/mnt/sd0/test/6", O_WRONLY|O_CREAT);
	if (fd < 0)
	{
		perror("cant open 6 file");
		return 1;
	}


	return 0;
}
