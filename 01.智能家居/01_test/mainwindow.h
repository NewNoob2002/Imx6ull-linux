#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtCore/QCoreApplication>
#include <linux/input.h>
#include <QSlider>
#include <uart_thread.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void on_horizontalSlider_sliderReleased();

    void on_horizontalSlider_valueChanged(int value);

public slots:
    void ap3216_timeout();
    void beep_timeout();

    void uart_to_main( char* module,  char*  value);


private:
    Ui::MainWindow *ui;

    int led_fd, beep_fd, ap3216_fd, dht11_fd,
        jdq_fd, sg90_fd, sr501_fd, bluetooth_fd;
    int sr501_flags;
    int jdq_status = 0;

    QSlider *slider = new QSlider();

    QString led_dev = "/dev/led";
    QString beep_dev = "/dev/beep";
    QString ap3216_dev = "/dev/ap3216";
    QString dht11_dev = "/dev/dht11";
    QString jdq_dev = "/dev/jdq";
    QString sg90_dev = "/dev/sg90";
    QString sr501_dev = "/dev/sr501";
    QString bluetooth_dev = "/dev/ttymxc2";

    FILE *raw_fd, *scale_fd;
    const char *voltage1_raw = "/sys/bus/iio/devices/iio:device0/in_voltage1_raw";
    const char *voltage_scale = "/sys/bus/iio/devices/iio:device0/in_voltage_scale";

    unsigned char buf[10];

    QTimer *ap3216c_tim;
    QTimer *beep_tim;

    QMouseEvent *event;

    Uart_Thread *uart_thread;

    /* 设置成静态成员函数
            C++中类的成员函数默认都提供了this指针，
            在非静态成员函数中当你调用函数的时候，
            编译器都会“自动”帮你把这个this指针加到函数形参里去
       但是 signal 的回调函数只有一个的参数，所以会报错：
            error: reference to non-static member function must be called*/
    static void sr501_handler(int signum);

    int open_port(const char *com, int nSpeed);

//private:
//    void mousePressEvent(QMouseEvent *event);

};
#endif // MAINWINDOW_H
