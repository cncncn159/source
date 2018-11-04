#ifndef CMSGBUSINESS_H
#define CMSGBUSINESS_H

//***********************************量革机的型号************************************************
// 	#define	LGJType 140		//量革机的型号		//特别说明：140型的图片借用150
// 	#define	LGJType 150		//量革机的型号
#define	LGJType 180u		//量革机的型号		//默认硬件版本
//	#define	LGJType 200		//量革机的型号
//	#define	LGJType 220		//量革机的型号
//	#define	LGJType 240		//量革机的型号
//	#define	LGJType 256		//量革机的型号
//	#define	LGJType 280		//量革机的型号
//	#define	LGJType 300		//量革机的型号
//	#define	LGJType 320		//量革机的型号

#include <QObject>

#define FRAME_HEX_HEADER 	0x68u
#define FRAME_HEX_TAIL		0x7fu

#define SOFEWAVE_VERSION	0x00
#define FRAME_LEN_DIFF		4u

#define TIMER_TIMEOUT		1000u
#define HEART_WAIT_TIME		5u

typedef struct
{
	uint8_t len;
	uint8_t data[50];
}sFrameData;

typedef struct
{
	uint8_t 	header;	//帧头
	uint16_t	len;	//帧长度
	eFrameType 	type;	//帧类型
	uint8_t		version;//版本号
	uint16_t	hard;	//硬件
	sFrameData	FrameData;
	uint8_t		tail;	//帧尾
}sFrame;

typedef enum
{
	E_FRAME_HANDSHAKE = 1,				//握手
	E_FRAME_HEARTBEAT,					//心跳
	E_FRAME_DETECTION,					//检测
	E_FRAME_MEASURING,					//测量中
	E_FRAME_END_MEASURE,				//测量结束
	E_FRAME_DATA_MEASURE,				//测量数据
	E_FRAME_INQUIRE_MEASURE				//测量查询
}eFrameType;							//帧类型

typedef enum
{
	E_INIT_STAGE,						//初始化状态
	E_WORK_STAGE						//工作状态
}eWorkingStage;

//class Message;
class CMsgBusiness : public QObject
{
    Q_OBJECT
public:
    explicit CMsgBusiness(QObject *parent = 0);
    ~CMsgBusiness();
    //这里可以在UI里来直接调用
    QString get()const;
	virtual void timerEvent(QTimerEvent *event);

private:
	sFrame protocol_data;
	uint8_t dataDeal(QByteArray);//接收到的帧数据解析处理
	uint8_t appDeal(sFrame frame);//接收到数据后的操作
	uint8_t versionJudgement(sFrame frame);//判断软件硬件版本号
	eWorkingStage work_stage;
	uint16_t timer_id;
	uint16_t heart_time;
	/*发送部分*/
	sFrame sedFrame;
	QByteArray sedArry;
	void sedDeal(eFrameType type,sFrameData data);//发送结构体赋值函数
	void sedFramePack(sFrame frame);//结构体打包成数组帧函数
signals:
    //处理完的Msg发到界面上去显示
    void signalSomethingComing(QByteArray);
	//下发命令到comm层
	void signalToSend(QByteArray);
private slots:
    //收到下层来的msg
    void onCommingMsg(const QByteArray);
	//收到界面上的动作
	void onCommingAct(const QByteArray);
public slots:
};



#endif // CMSGBUSINESS_H
