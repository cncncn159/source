#include "uartthread.h"
#include <QDebug>

UartThread::UartThread(QObject* parent):
    QThread(parent)
{
    stopped = false;
    initPort();

}

void UartThread::run()
{
    if(serial->waitForReadyRead())
    {
        QByteArray requestData = serial->readAll();
        while (serial->waitForReadyRead(10))
            requestData += serial->readAll();
        emit readyRead(requestData);
    }
}

void UartThread::initPort()
{

    serial = new QSerialPort(this);

    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),this, &UartThread::handleError);
    QObject::connect(serial,SIGNAL(readyRead()),this,SLOT(recSerialData()));
    serial->setPortName("COM3");
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setParity(QSerialPort::NoParity);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if(serial->open(QIODevice::ReadWrite))
    {
        qDebug()<<"open success";
    }
    else
    {
        qDebug()<<"open failed";
    }

}

void UartThread::recSerialData()
{
    QByteArray ba;
    ba = serial->readAll();
    qDebug()<<"receive info:"<<ba.toHex();
    //ui->textEdit->setText(ba);
    ba.clear();
}

void UartThread::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError)
    {
        //QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

void UartThread::closeSerialPort()
{

    if (serial->isOpen())
        serial->close(); //关闭串口

}
