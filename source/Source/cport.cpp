#include "cport.h"
#include "Message.h"
#include <QDebug>

CPort::CPort(QObject *parent) : QObject(parent)
{
    port = new QSerialPort(this);
#ifdef __arm__
    port->setPortName("/dev/ttymxcl");
#else
    port->setPortName("COM8");
#endif
    port->setBaudRate(QSerialPort::Baud9600);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);

    connect(port,SIGNAL(readyRead()),this,SLOT(onReadData()));
    if(!port->open(QIODevice::ReadWrite))
    {
        qDebug()<<"Port Can't open";
    }

    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer,SIGNAL(timeout()),SLOT(onParse()));
    timer->start();

    this->moveToThread(&worker);
    worker.start(QThread::HighPriority);
}

CPort::~CPort()
{

}

void CPort::onDownwardMsg(const QByteArray array)
{
    //QByteArray array(msg.content);

    port->write(array);
}

void CPort::onReadData()
{
    data.append(port->readAll());
}

void CPort::onParse()
{
	QByteArray msg;


    //for(int i = 0;i<data.size();i++)
	if(data.size() != 0)
    {
        //...此处加解析
		msg = data;
		emit signalUpwardMsg(msg);
		data.clear();
    }
	
}
