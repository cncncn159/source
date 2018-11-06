#include "cmsgbusiness.h"


CMsgBusiness::CMsgBusiness(QObject *parent) : QObject(parent)
{
	sedArry.resize(200u);
	timer_heart = new QTimer(this);
	timer_hands = new QTimer(this);
	connect(timer_heart, SIGNAL(timeout()), this, SLOT(timerOutHeart()));
	connect(timer_hands, SIGNAL(timeout()), this, SLOT(timerOuthands()));
	timer_hands->start(HANDS_POLL_TIME);
	work_stage = E_INIT_STAGE;
	protocol_data = { 0,0,static_cast<eFrameType>(0),0,0,0,0 };
	heart_lost_cnt = 0;
	dismsg = "cn";
	scan_data.resize(LGJUpBound+5u);//5为预留
	scan_data.clear();
	sedFrame = { 0,0,static_cast<eFrameType>(0),0,0,0,0 };
	sedArry = "cn";
}
CMsgBusiness::~CMsgBusiness()
{
	
}
/*串口收到数据*/
void CMsgBusiness::onCommingMsg(const QByteArray msg)
{   //这里可以对收到的msg进行处理
	//QString str = msg;
	if (dataDeal(msg) != 0)
	{
		appDeal(protocol_data);
	}
}



QString CMsgBusiness::get(void) const
{
	QString str = "cncncnncnc";
	return str;
}

/*发送帧处理*/
void CMsgBusiness::sedDeal(eFrameType type,sFrameData data)
{
	sedFrame.header = FRAME_HEX_HEADER;
	sedFrame.version = SOFEWAVE_VERSION;
	sedFrame.hard = LGJType;
	sedFrame.tail = FRAME_HEX_TAIL;
	switch (type)
	{
	case E_FRAME_HEARTBEAT:
	{
		sedFrame.len = 0x04u;
		sedFrame.type = type;
		sedFrame.FrameData.len = sedFrame.len - FRAME_LEN_DIFF;
		//心跳帧不包含数据
		break;
	}
	case E_FRAME_HANDSHAKE:
	{
		sedFrame.len = 0x04u;
		sedFrame.type = type;
		sedFrame.FrameData.len = sedFrame.len - FRAME_LEN_DIFF;
		//握手帧不包含数据
		break;
	}
	default:
		break;
	}
}
/*帧打包函数*/
void CMsgBusiness::sedFramePack(sFrame frame)
{
	
	sedArry.clear();
	uint16_t len = frame.len + 4u;
	if (len > 500u)	//发送帧大小限制
	{
		return;
	}
	sedArry.resize(len);
	sedArry.data()[0] = frame.header;
	sedArry.data()[1] = static_cast<uint8_t>(frame.len >> 8u);
	sedArry.data()[2] = static_cast<uint8_t>(frame.len);
	sedArry.data()[3] = static_cast<uint8_t>(frame.type);
	sedArry.data()[4] = frame.version;
	sedArry.data()[5] = static_cast<uint8_t>(frame.hard >> 8u);
	sedArry.data()[6] = static_cast<uint8_t>(frame.hard);
	if ((frame.FrameData.len != 0)&&(frame.FrameData.len+7u<=len))
	{
		for (uint16_t i = 0; i < frame.FrameData.len; i++)
		{
			sedArry.data()[7 + i] = frame.FrameData.data[i];
		}
	}
	sedArry.data()[frame.FrameData.len + 7u] = frame.tail;
}
/* 返回1：成功收到数据 0：解析失败*/
uint8_t CMsgBusiness::dataDeal(QByteArray msg)
{
	//此处增加数据处理
	int16_t idx = -1;
	uint16_t len = 0,hard = 0;
	uint8_t ret = 0;
	int16_t i;

	if (msg.size() > 200)	//限制一帧数不能大于200字节
	{
		return 0;
	}
	FrameClear(&protocol_data);
	for (i = 0; i < msg.size(); i++)
	{
		if (msg.at(i) == FRAME_HEX_HEADER)
		{
			idx = i;
			break;
		}
	}
	if((idx >= 0)&&(idx<msg.size()))
	{
		protocol_data.header = msg.at(idx);
		len |= 0xff00&(static_cast<uint16_t>(msg.at(idx + 1)) << 8u);
		len |= 0x00ff&(static_cast<uint16_t>(msg.at(idx + 2)));
		if ((len+ FRAME_LEN_DIFF) > msg.size())
		{
			return 0;
		}
		protocol_data.len = len;
		qDebug() << msg;
		if (msg.size() >= (idx + len + 3u))
		{
			if (msg.at(idx + len + 3u) == FRAME_HEX_TAIL)
			{
				ret = 1;//帧校验通过
			}
			if (len < 4)
			{
				ret = 0;//最小帧长度
			}
		}
		if (ret == 1)
		{//帧处理操作
			protocol_data.type = static_cast<eFrameType>(msg.at(idx + 3u));
			protocol_data.version = msg.at(idx + 4u);
			hard |= (0xff00&(static_cast<uint16_t>(msg.at(idx + 5u)) << 8u));
			hard |= (0x00ff&(static_cast<uint16_t>(msg.at(idx + 6u))));
			protocol_data.hard = hard;
			protocol_data.tail = msg.at(idx + len + 3u);
			protocol_data.FrameData.len = len - 4u;
			if (protocol_data.FrameData.len != 0)
			{
				for (i = 0; i < protocol_data.FrameData.len; i++)
				{
					protocol_data.FrameData.data[i] = msg.at(idx + 7u + i);
				}
			}
			if ((protocol_data.type >= E_FRAME_HANDSHAKE) \
				&& (protocol_data.type <= E_FRAME_INQUIRE_MEASURE))
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
		}
		return ret;
	}
	return 0;
}
/*接收部分
收到数据 处理函数*/
uint8_t CMsgBusiness::appDeal(sFrame frame)
{
	uint8_t ret = 0;
	uint8_t type = frame.type;

	switch (type)
	{
	case E_FRAME_HANDSHAKE:
	{
		if (work_stage == E_INIT_STAGE)
		{
			if (versionJudgement(frame) == 1u)
			{
				if (frame.FrameData.data[0] == 1u)
				{//握手成功
					dismsg.clear();
					dismsg.append("connect success");
					work_stage = E_WORK_STAGE;
					emit signalSomethingComing(dismsg);
					timer_heart->start(HEART_POLL_TIME);
					if(timer_hands->isActive())
					{
						timer_hands->stop();
					}
				}
				else
				{
					//需要增加错误处理
				}
			}
		}
		else if (work_stage == E_WORK_STAGE)
		{
			//工作状态收到握手不处理
		}
		else
		{}
		break;
	}
	case E_FRAME_HEARTBEAT:
	{
		if (work_stage == E_INIT_STAGE)
		{
			//do nothing
		}
		else if (work_stage == E_WORK_STAGE)
		{
			//收到心跳
			if (heart_lost_cnt > 0)
			{
				heart_lost_cnt--;
			}
		}
		break;
	}
	case E_FRAME_DETECTION:
	{
		if (work_stage == E_WORK_STAGE)
		{
			if (versionJudgement(frame) == 1u)
			{
				if (frame.FrameData.data[0] == 0x02u)//请求字节为回复
				{
					if (frame.FrameData.len == LGJUpBound+1)//长度校对
					{
						scan_data.clear();
						for (uint8_t i = 1; i < LGJUpBound + 1; i++)
						{
							scan_data.append(frame.FrameData.data[i]);
						}
						//emit signalSomethingComing(scan_data);
					}
				}
			}
			else
			{
				//硬件版本错误
			}
		}
		break;
	}
	default:
		break;
	}
	return ret;
}

uint8_t CMsgBusiness::versionJudgement(sFrame frame)
{
	uint8_t ret = 0;
	/*暂时只判断硬件版本*/
	if (frame.hard == LGJType)
	{
		ret = 1u;
	}
	else
	{
		ret = 0;
	}
	return ret;
}

void CMsgBusiness::onCommingAct(const QByteArray)
{

}
/*心跳发送定时到达函数*/
void CMsgBusiness::timerOutHeart(void)
{
	sFrameData seddata = { 0,0 };//要发送的数据

	if (heart_lost_cnt >= HEART_LOST_CNT)
	{//收不到心跳断连
		work_stage = E_INIT_STAGE;
		timer_heart->stop();
		timer_hands->start(HANDS_POLL_TIME);
		heart_lost_cnt = 0;
		dismsg.clear();
		dismsg.append("disconnect");
		emit signalSomethingComing(dismsg);

	}
	else
	{
		sedDeal(E_FRAME_HEARTBEAT, seddata);//发送结构体赋值函数
		sedFramePack(sedFrame);//结构体打包成数组帧函数
		emit signalToSend(sedArry);// 发送信号
		heart_lost_cnt++;
	}
}
/*握手发送定时器到达函数*/
void CMsgBusiness::timerOuthands(void)
{
	sFrameData seddata = { 0,0 };//要发送的数据

	sedDeal(E_FRAME_HANDSHAKE, seddata);//发送结构体赋值函数
	sedFramePack(sedFrame);//结构体打包成数组帧函数
	emit signalToSend(sedArry);// 发送信号
}

void CMsgBusiness::FrameClear(sFrame* frame)
{
	frame->header = 0;
	frame->len = 0;
	frame->type = static_cast<eFrameType>(0);
	frame->version = 0;
	frame->hard = 0;
	frame->FrameData.len = 0;
	frame->tail = 0;
}
