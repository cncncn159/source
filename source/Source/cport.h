#ifndef CPORT_H
#define CPORT_H

#include <QObject>
#include <QSerialPort>
#include <QThread>
#include <QTimer>

//class Message;

class CPort : public QObject
{
    Q_OBJECT
public:
    explicit CPort(QObject *parent = 0);
    ~CPort();
signals:
    //向上层传送消息
	void signalUpwardMsg(const QByteArray);

private slots:
    //上层传过来的消息发送到串口
	void onDownwardMsg(const QByteArray);
	void onReadData();
	//读串口数据

	void onParse();
	//在这里定时的解析收到的数据
private:
    QSerialPort* port;
    QThread worker;
    QTimer* timer;
    QByteArray data;
};

#endif // CPORT_H
