#ifndef CMSGBUSINESS_H
#define CMSGBUSINESS_H

#include <QObject>

#define FRAME_HEX_HEADER 	0x68
#define FRAME_HEX_TAIL		0x7f 
#define TIMER_TIMEOUT		1000u
#define HEART_WAIT_TIME		5u

typedef struct
{
	uint8_t len;
	uint8_t data[50];
}sFrameData;

typedef struct
{
	uint8_t 	header;	//֡ͷ
	uint16_t	len;	//֡����
	eFrameType 	type;	//֡����
	uint8_t		version;//�汾��
	uint16_t	hard;	//Ӳ��
	sFrameData	FrameData;
	uint8_t		tail;	//֡β
}sFrame;

typedef enum
{
	E_FRAME_HANDSHAKE = 1,				//����
	E_FRAME_HEARTBEAT,					//����
	E_FRAME_DETECTION,					//���
	E_FRAME_MEASURING,					//������
	E_FRAME_END_MEASURE,				//��������
	E_FRAME_DATA_MEASURE,				//��������
	E_FRAME_INQUIRE_MEASURE				//������ѯ
}eFrameType;							//֡����

typedef enum
{
	E_INIT_STAGE,						//��ʼ��״̬
	E_WORK_STAGE						//����״̬
}eWorkingStage;

//class Message;
class CMsgBusiness : public QObject
{
    Q_OBJECT
public:
    explicit CMsgBusiness(QObject *parent = 0);
    ~CMsgBusiness();
    //���������UI����ֱ�ӵ���
    QString get()const;
	virtual void timerEvent(QTimerEvent *event);

private:
	sFrame protocol_data;
	uint8_t dataDeal(QByteArray);
	uint8_t appDeal(sFrame frame);
	eWorkingStage work_stage;
	uint16_t timer_id;
	uint16_t heart_time;
signals:
    //�������Msg����������ȥ��ʾ
    void signalSomethingComing(QByteArray);
	//�·����comm��
	void signalToSend(QByteArray);
private slots:
    //�յ��²�����msg
    void onCommingMsg(const QByteArray);
	//�յ������ϵĶ���
	void onCommingAct(const QByteArray);
public slots:
};



#endif // CMSGBUSINESS_H
