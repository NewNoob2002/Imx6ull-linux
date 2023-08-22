#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <poll.h>


int fd;

void handler(int signum)
{
	close(fd);		
	exit(0);
	return ;
}

void sr501_handler(int signum)
{
	char val;
	/* 读取sr501值 */
	read(fd, &val, 1);

	printf("val is %d, %s\n", val, val==1?"have people":"no people");
}

int main(int argc, char *argv[])
{
	char buf[10];
	
	int val;
	char status = 1;
	int flags;
	int tim;

	signal(SIGINT, handler);  /* ctrl+c */
	signal(SIGIO, sr501_handler);
	
	fd = open("/dev/sr501", O_RDWR);
	if(fd < 0){
		printf("/dev/sr501 open failed\n");
		return 1;
	}

	/* F_SETOWN: 设置将接收SIGIO和SIGURG信号的进程id */
	fcntl(fd, F_SETOWN, getpid());
	
	/*  F_GETFL: 取得fd的文件状态标志 */
	flags = fcntl(fd, F_GETFL);
	
	/* O_ASYNC: 当I/O可用的时候,允许SIGIO信号发送到进程组,
	   例如:当有数据可以读的时候 */
	fcntl(fd, F_SETFL, flags | O_ASYNC);  //启动驱动的fasync功能

	while(1){
		sleep(1);
	}

	
	return 0;
}

