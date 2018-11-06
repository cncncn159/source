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
    //���ϲ㴫����Ϣ
	void signalUpwardMsg(const QByteArray);

private slots:

	//����������

	void onParse();
	//�����ﶨʱ�Ľ����յ�������
public slots:
	void onDownwardMsg(const QByteArray);
	//�ϲ㴫��������Ϣ���͵�����
	void onReadData();
private:
    QSerialPort* port;
    QThread worker;
    QTimer* timer;
    QByteArray data;
};

#endif // CPORT_H
