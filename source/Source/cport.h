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
    //�ϲ㴫��������Ϣ���͵�����
	void onDownwardMsg(const QByteArray);
	void onReadData();
	//����������

	void onParse();
	//�����ﶨʱ�Ľ����յ�������
private:
    QSerialPort* port;
    QThread worker;
    QTimer* timer;
    QByteArray data;
};

#endif // CPORT_H
