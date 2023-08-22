#include "uart_thread.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <mainwindow.h>


Uart_Thread::Uart_Thread(int bluetooth_fd)
{
    this->bluetooth_fd = bluetooth_fd;
}

void Uart_Thread::run()
{
    int cnt = 0;
    char buf[20] = {0};
    char module[10] = {0};
    char value[10] = {0};
    printf("uart_recv\n");
    while(1){
        read(bluetooth_fd, &buf[cnt], 1);
        cnt++;
        /* 如果包含右括号，说明接收完一帧数据 */
        if(strstr(buf, "}")){
            printf("uart recv = %s\n", buf);
            sscanf(buf, "{%[^,],%[^,]}", module, value);
            printf("module = %s, value = %s\n", module, value);
            cnt = 20;
            emit to_main(module, value);
        }
        if(cnt == 20){
            cnt = 0;
            memset(buf, 0, sizeof(buf));
        }
    }
}
