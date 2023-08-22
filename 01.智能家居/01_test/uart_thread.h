#ifndef UART_THREAD_H
#define UART_THREAD_H

#include <QThread>
#include <QObject>

class Uart_Thread : public QThread
{
    Q_OBJECT

public:
    Uart_Thread(int bluetooth_fd);
    ~Uart_Thread() {};

private:
    int bluetooth_fd;

protected:
    void run() override;

signals:
    void to_main(char* , char* );

};

#endif // UART_THREAD_H
