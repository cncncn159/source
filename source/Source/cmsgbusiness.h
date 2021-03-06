#ifndef CMSGBUSINESS_H
#define CMSGBUSINESS_H
#include <QTimer>
#include <QDebug>
#include <QBytearray>
#include <QObject>
#include "sys.h"

#define FRAME_HEX_HEADER 	0x68u
#define FRAME_HEX_TAIL		0x7fu

#define SOFEWAVE_VERSION	0x00
#define FRAME_LEN_DIFF		4u

#define HEART_POLL_TIME		5000u		//心跳轮询发送时间 单位ms
#define HANDS_POLL_TIME		1000u		//握手轮询发送时间 单位ms

#define HEART_LOST_CNT		5u			//心跳丢失次数执行 断连
typedef struct
{
	uint8_t len;
	uint8_t data[50];
}sFrameData;

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
private:
	sFrame protocol_data;
	uint8_t dataDeal(QByteArray);//接收到的帧数据解析处理
	uint8_t appDeal(sFrame frame);//接收到数据后的操作
	uint8_t versionJudgement(sFrame frame);//判断软件硬件版本号
	eWorkingStage work_stage;
	uint8_t heart_lost_cnt;//心跳遗失次数
	/*发向显示的消息*/
	QByteArray dismsg;
	QByteArray scan_data;//扫描数据 发往显示
	/*QTimer test*/
	QTimer* timer_heart;//心跳轮询发送定时器
	QTimer* timer_hands;//握手轮询发送定时器
	/*发送部分*/
	sFrame sedFrame;
	QByteArray sedArry;
	void sedDeal(eFrameType type,sFrameData data);//发送结构体赋值函数
	void sedFramePack(sFrame frame);//结构体打包成数组帧函数
	void FrameClear(sFrame* frame);//清空结构体函数
	/*接收标志*/
	uint8_t rec_flag;// 0 初始值 1 接收完成 2 收到测量中 3 收到测量结束
	//uint16_t lenth;//接收到测量结束的line长度
	/*皮参数*/
	uint16_t accu_area;	//每张皮的面积点数

signals:
    //处理完的Msg发到界面上去显示
    void signalSomethingComing(QByteArray);
	//下发命令到comm层
	void signalToSend(QByteArray);
private slots:
	void timerOutHeart(void);
	void timerOuthands(void);
	
public slots:

	//收到下层来的msg
	void onCommingMsg(const QByteArray);
	//收到界面上的动作
	void onCommingAct(const QByteArray);
	//上层发来的扫描信号
	void onScanSend(void);
};



#endif // CMSGBUSINESS_H
