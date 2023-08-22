#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/signal.h>
#include <QDebug>
#include <QMouseEvent>

#include <thread>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>


/* 给静态成员函数：sr501_handler 提供全局变量
   否则无法访问到类中的非静态成员变量*/
int temp_sr501_fd;
Ui::MainWindow *temp_ui;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    temp_ui = ui;

    /* 设置滑动条控制范围 */
    ui->horizontalSlider->setRange(0, 100);
    /* 设置滑动条控制步长 */
    ui->horizontalSlider->setSingleStep(0);
    /* 关闭滑块跟踪：滑块仅在用户释放滑块时才发出valueChanged()信号 */
    ui->horizontalSlider->setTracking(false);

    /*open led*/
    led_fd = open(led_dev.toStdString().c_str(), O_RDWR);
    if(led_fd < 0)
        printf("open led failed\n");

    /*open beep*/
    beep_fd = open(beep_dev.toStdString().c_str(), O_RDWR);
    if(beep_fd < 0)
        printf("open beep failed\n");

    /*open ap3216*/
    ap3216_fd = open(ap3216_dev.toStdString().c_str(), O_RDWR);
    if(ap3216_fd < 0)
        printf("open ap3216 failed\n");

    /*open dht11*/
    dht11_fd = open(dht11_dev.toStdString().c_str(), O_RDONLY);
    if(dht11_fd < 0)
        printf("open dht11 failed\n");

    /*open jdq*/
    jdq_fd = open(jdq_dev.toStdString().c_str(), O_RDWR);
    if(jdq_fd < 0)
        printf("open jdq failed\n");

    /*open sg90*/
    sg90_fd = open(sg90_dev.toStdString().c_str(), O_WRONLY);
    if(sg90_fd < 0)
        printf("open sg90 failed\n");

    /*open sr501*/
    sr501_fd = open(sr501_dev.toStdString().c_str(), O_RDWR);
    if(sr501_fd < 0)
        printf("open sr501 failed\n");
    else{
        temp_sr501_fd = sr501_fd;
        // 在文件描述符上设置异步通知
        /* F_SETOWN: 设置将接收SIGIO和SIGURG信号的进程id */
        fcntl(sr501_fd, F_SETOWN, getpid());
        /*  F_GETFL: 取得fd的文件状态标志 */
        sr501_flags = fcntl(sr501_fd, F_GETFL);
        /* O_ASYNC: 当I/O可用的时候,允许SIGIO信号发送到进程组,
           例如:当有数据可以读的时候 */
        fcntl(sr501_fd, F_SETFL, sr501_flags | O_ASYNC);  //启动驱动的fasync功能

        signal(SIGIO, sr501_handler);
    }

    /* init uart */
    bluetooth_fd = open_port(bluetooth_dev.toStdString().c_str(), 9600);
    if(bluetooth_fd < 0)
        printf("open sr501 failed\n");
    /* 创建串口接收线程 */
    uart_thread = new Uart_Thread(bluetooth_fd);
    connect(uart_thread, SIGNAL(to_main(char* , char* )),
            this, SLOT(uart_to_main(char* , char* )));

    uart_thread->start();

    /*timer*/
    ap3216c_tim = new QTimer();
    beep_tim = new QTimer();
    ap3216c_tim->setInterval(300);
    beep_tim->setInterval(1000);
    connect(ap3216c_tim,SIGNAL(timeout()),this,SLOT(ap3216_timeout()));
    connect(beep_tim,SIGNAL(timeout()),this,SLOT(beep_timeout()));
    ap3216c_tim->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}


int MainWindow::open_port(const char *com, int nSpeed)
{
    int fd;
    //fd = open(com, O_RDWR|O_NOCTTY|O_NDELAY);
    fd = open(com, O_RDWR|O_NOCTTY);
    if (-1 == fd){
        return(-1);
    }

    if(fcntl(fd, F_SETFL, 0)<0) /* 设置串口为阻塞状态*/
    {
        printf("fcntl failed!\n");
        return -1;
    }

    struct termios newtio,oldtio;

    if ( tcgetattr( fd,&oldtio) != 0) {
        perror("SetupSerial 1");
        return -1;
    }

    bzero( &newtio, sizeof( newtio ) );
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    newtio.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    newtio.c_oflag  &= ~OPOST;   /*Output*/


    //8位数据位
    newtio.c_cflag |= CS8;

    //无校验位
    newtio.c_cflag &= ~PARENB;


    switch( nSpeed )
    {
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
        break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
        break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
        break;
    }

    //1位停止位
    newtio.c_cflag &= ~CSTOPB;

    newtio.c_cc[VMIN]  = 1;  /* 读数据时的最小字节数: 没读到这些数据我就不返回! */
    newtio.c_cc[VTIME] = 0; /* 等待第1个数据的时间:
                             * 比如VMIN设为10表示至少读到10个数据才返回,
                             * 但是没有数据总不能一直等吧? 可以设置VTIME(单位是10秒)
                             * 假设VTIME=1，表示:
                             *    10秒内一个数据都没有的话就返回
                             *    如果10秒内至少读到了1个字节，那就继续等待，完全读到VMIN个数据再返回
                             */
    tcflush(fd,TCIFLUSH);

    return fd;
}

void MainWindow::uart_to_main(char* module, char*  value){
    int val;
//    printf("uart_to_main, module = %s, value = %s\n", module, value);
    /* 控制窗帘：舵机 */
    if(strcmp(module, "curtain") == 0){
        if(atoi(value) >= 0 && atoi(value) <= 180){
            val = 100.0 / 180 * atoi(value);
            ui->horizontalSlider->setValue(val);
        }
    }
    /* 控制门锁：继电器 */
    else if(strcmp(module, "door") == 0){
        if(atoi(value) == 0 || atoi(value) <= 1){
            val = atoi(value);
            jdq_status = val;
            write(jdq_fd, &val, 1);
        }
    }
    /* 控制灯光：led */
    else if(strcmp(module, "led") == 0){
        if(atoi(value) == 0 || atoi(value) <= 1){
            val = atoi(value);
            write(led_fd, &val, 1);
        }
    }
    /* 控制门铃：beep */
    else if(strcmp(module, "beep") == 0){
        if(atoi(value) == 0 || atoi(value) <= 1){
            val = atoi(value);
            /* start beep_tim */
            beep_tim->start();
            write(beep_fd, &val, 1);
        }
    }
}

/* sr501 */
void MainWindow::sr501_handler(int signum)
{
    char val;
    /* 读取sr501值 */
    read(temp_sr501_fd, &val, 1);
//    printf("val is %d, %s\n", val, val==1?"have people":"no people");
    temp_ui->label_people->setNum(val);
}


/* timeout beep */
void MainWindow::beep_timeout()
{
    /* disable beep */
    buf[0] = 0;
    write(beep_fd, buf, 1);
    /* stop beep_tim */
    beep_tim->stop();
}

/* timeout read ap3216 and dht11 and adc */
void MainWindow::ap3216_timeout()
{
    unsigned short ir, light, dis;
    static int cnt = 0;
    int raw = 300;
//    double scale;
    char data[20] = {0};

    /* 1.5秒读一次adc */
    if(cnt % 5 == 0){
        /* 打开adc文件 */
        raw_fd = fopen(voltage1_raw, "r");
        if(raw_fd == NULL){
            printf("open raw_fd failed!\n");
        }else{
            rewind(raw_fd);   // 将光标移回文件开头
            fscanf(raw_fd, "%s", data);
            raw = atoi(data);
            //一定要关闭文件，否则占用文件后内核无法更新ADC值
            fclose(raw_fd);
        }

        //        scale_fd = fopen(voltage_scale, "r");
        //        if(scale_fd == NULL){
        //            printf("open scale_fd failed!\n");
        //        }else{
        //            memset(data, 0, sizeof(data));
        //            rewind(scale_fd);   // 将光标移回文件开头
        //            fscanf(scale_fd, "%s", data);
        //            scale = atof(data);
//                     fclose(scale_fd);
        //        }

        ui->label_air->setNum(raw);
    }

    /* 3秒读取一次dht11 */
    if(cnt % 10 == 0){
        cnt = 0;
        /* 读dht11 */
        read(dht11_fd, buf, 5);
        ui->label_tmp->setNum(buf[2]);
        ui->label_hum->setNum(buf[0]);
    }
    cnt++;

    /* 0.3s读取ap3216c */
    read(ap3216_fd, &buf[1], 6);
    if(buf[1] & 0X80)     /* IR_OF位为1,则数据无效 */
       ir = 0;
    else
       ir = (buf[1] << 2) | (buf[2] & 0x03);   /* 读取IR传感器的数据 */

    light = (buf[3] << 8) | buf[4];          /* 读取ALS传感器的数据 */

    if(buf[5] & 0x40)    /* IR_OF位为1,则数据无效 */
        dis = 0;
    else
        dis = ((buf[5] & 0x3f) << 4) | (buf[6] & 0x0f);   /* 读取PS传感器的数据 */

    ui->label_ir->setNum(ir);
    ui->label_light->setNum(light);
    ui->label_dis->setNum(dis);
}

/* control led */
void MainWindow::on_pushButton_clicked()
{
    read(led_fd, buf, 1);
    buf[0] = buf[0] == 1 ? 0 : 1;
    write(led_fd, buf, 1);
}

/* control beep */
void MainWindow::on_pushButton_2_clicked()
{
    /* start beep_tim */
    beep_tim->start();

    /* enable beep */
    buf[0] = 1;
    write(beep_fd, buf, 1);
}

/* control jdq */
void MainWindow::on_pushButton_4_clicked()
{
    jdq_status ^= 1;
    buf[0] = jdq_status;
    write(jdq_fd, buf, 1);
}

/* 窗帘控制：滑动 */
void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    ui->progressBar->setValue(position);
}

/* 窗帘控制：释放 */
void MainWindow::on_horizontalSlider_sliderReleased()
{
//    int buf[1];
//    buf[0] = ui->horizontalSlider->value();
//    printf("sliderReleased  %d\n", buf[0]);
//    buf[0] = 180 / 100.0 * buf[0];
//    write(sg90_fd, buf, 1);
}

/* 窗帘控制：数值变化时被调用（滑块仅在用户释放滑块时才发出valueChanged()信号） */
void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    int buf[1];
    ui->progressBar->setValue(value);
    buf[0] =  ui->progressBar->value();
    buf[0] = 180 / 100.0 * buf[0];
    write(sg90_fd, buf, 1);
//    printf("valueChanged  %d\n", value);
}
