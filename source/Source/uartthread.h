#ifndef UARTTHREAD_H
#define UARTTHREAD_H
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include "scantest.h"

class UartThread : public QThread
{
    Q_OBJECT
public:
    explicit UartThread(QObject* parent = 0);
    void stop();

protected:
    void run();
private:
    QSerialPort *serial;
    volatile bool stopped;
    void initPort();
    void handleError(QSerialPort::SerialPortError error);
    void closeSerialPort();
private slots:
    void recSerialData(void);
signals:
    void readyRead(QByteArray);
};

#endif // UARTTHREAD_H
