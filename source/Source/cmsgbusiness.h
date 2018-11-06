#ifndef CMSGBUSINESS_H
#define CMSGBUSINESS_H
#include <QTimer>
//***********************************��������ͺ�************************************************
// 	#define	LGJType 140		//��������ͺ�		//�ر�˵����140�͵�ͼƬ����150
// 	#define	LGJType 150		//��������ͺ�
#define	LGJType 180u		//��������ͺ�		//Ĭ��Ӳ���汾
//	#define	LGJType 200		//��������ͺ�
//	#define	LGJType 220		//��������ͺ�
//	#define	LGJType 240		//��������ͺ�
//	#define	LGJType 256		//��������ͺ�
//	#define	LGJType 280		//��������ͺ�
//	#define	LGJType 300		//��������ͺ�
//	#define	LGJType 320		//��������ͺ�

#include <QObject>

#define FRAME_HEX_HEADER 	0x68u
#define FRAME_HEX_TAIL		0x7fu

#define SOFEWAVE_VERSION	0x00
#define FRAME_LEN_DIFF		4u

#define HEART_POLL_TIME		1000u		//������ѯ����ʱ�� ��λms
#define HANDS_POLL_TIME		5000u		//������ѯ����ʱ�� ��λms

#define HEART_LOST_CNT		5u			//������ʧ����ִ�� ����
typedef struct
{
	uint8_t len;
	uint8_t data[50];
}sFrameData;

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

private:
	sFrame protocol_data;
	uint8_t dataDeal(QByteArray);//���յ���֡���ݽ�������
	uint8_t appDeal(sFrame frame);//���յ����ݺ�Ĳ���
	uint8_t versionJudgement(sFrame frame);//�ж����Ӳ���汾��
	eWorkingStage work_stage;
	uint8_t heart_lost_cnt;//������ʧ����
	/*������ʾ����Ϣ*/
	QByteArray dismsg;
	/*QTimer test*/
	QTimer* timer_heart;//������ѯ���Ͷ�ʱ��
	QTimer* timer_hands;//������ѯ���Ͷ�ʱ��
	/*���Ͳ���*/
	sFrame sedFrame;
	QByteArray sedArry;
	void sedDeal(eFrameType type,sFrameData data);//���ͽṹ�帳ֵ����
	void sedFramePack(sFrame frame);//�ṹ����������֡����
signals:
    //�������Msg����������ȥ��ʾ
    void signalSomethingComing(QByteArray);
	//�·����comm��
	void signalToSend(QByteArray);
private slots:
	void timerOutHeart(void);
	void timerOuthands(void);
	
public slots:

	//�յ��²�����msg
	void onCommingMsg(const QByteArray);
	//�յ������ϵĶ���
	void onCommingAct(const QByteArray);
};



#endif // CMSGBUSINESS_H
