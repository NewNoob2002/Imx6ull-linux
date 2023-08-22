#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
	char data[6];
	int fd;

	fd = open("/dev/ap3216", O_RDWR);
	if(fd < 0)
	{
		printf("open /dev/ap3216 failed\n");
		return -1;
	}

	while(1)
	{
		read(fd, data, 6);
		printf("IR = %d, light = %d, dis = %d\n\n",
				(data[0] << 8) | data[1], 
				(data[2] << 8) | data[3],
				(data[4] << 8) | data[5]);
		sleep(1);
	}
	return 0;
}

