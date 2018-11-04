#include "cmsgbusiness.h"

#include <QTimerEvent>

CMsgBusiness::CMsgBusiness(QObject *parent) : QObject(parent)
{
	timer_id = this->startTimer(TIMER_TIMEOUT);
}
CMsgBusiness::~CMsgBusiness()
{
	if (timer_id != 0)
	{
		killTimer(timer_id);
	}
}

void CMsgBusiness::onCommingMsg(const QByteArray msg)
{   //这里可以对收到的msg进行处理
	//QString str = msg;
    emit signalSomethingComing(msg);
	if (dataDeal(msg) != 0)
	{
		appDeal(protocol_data);
	}
	emit signalToSend(msg);
}



QString CMsgBusiness::get(void) const
{
	QString str = "cncncnncnc";
	return str;
}
void CMsgBusiness::timerEvent(QTimerEvent * event)
{
	sFrameData seddata = { 0,0 };//要发送的数据

	if (event->timerId() == timer_id)
	{
		if (work_stage == E_WORK_STAGE)
		{
			if (heart_time <= 0xffffu)
			{
				heart_time++;
			}
			if (heart_time >= HEART_WAIT_TIME)
			{
				//发送心跳
				sedDeal(E_FRAME_HEARTBEAT, seddata);//发送结构体赋值函数
				sedFramePack(sedFrame);//结构体打包成数组帧函数
			}
		}
	}
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
		sedFrame.FrameData.len = sedFrame.len - FRAME_LEN_DIFF;
		//心跳帧不包含数据
		break;
	}
	default:
		break;
	}
}
/*帧打包函数*/
void CMsgBusiness::sedFramePack(sFrame frame)
{
	uint16_t len = frame.len + 4u;
	sedArry.resize(len);
	sedArry.data()[0] = frame.header;
	sedArry.data()[1] = static_cast<uint8_t>(frame.len);
	sedArry.data()[2] = static_cast<uint8_t>(frame.len >> 8u);
	sedArry.data()[3] = static_cast<uint8_t>(frame.type);
	sedArry.data()[4] = frame.version;
	sedArry.data()[5] = static_cast<uint8_t>(frame.hard);
	sedArry.data()[6] = static_cast<uint8_t>(frame.hard >> 8u);
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
	uint16_t idx;
	uint16_t len,hard;
	uint8_t ret = 0;
	uint16_t i;

	idx = msg.indexOf(FRAME_HEX_HEADER);
	if ( idx != -1)
	{
		protocol_data.header = msg.at(idx);
	}
	len = static_cast<uint16_t>(msg.at(idx + 1)) << 8u;
	len |= static_cast<uint16_t>(msg.at(idx + 2));
	protocol_data.len = len;

	if (msg.size >= idx + len + 3u)
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
		protocol_data.type = msg.at(idx + 3u);
		protocol_data.version = msg.at(idx + 4u);
		hard = static_cast<uint16_t>(msg.at(idx + 5u)) << 8u;
		hard |= static_cast<uint16_t>(msg.at(idx + 6u));
		protocol_data.hard = hard;
		protocol_data.tail = msg.at(idx + len + 3u);
		protocol_data.FrameData.len = len - 4u;
		if (protocol_data.FrameData.len != 0)
		{
			for (i = 0; i < protocol_data.FrameData.len; i++)
			{
				protocol_data.FrameData.data[i] = msg.at(idx + 8u);
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
/*收到数据 处理函数*/
uint8_t CMsgBusiness::appDeal(sFrame frame)
{
	uint8_t ret;
	uint8_t type = frame.type;

	switch (type)
	{
	case E_FRAME_HANDSHAKE:
	{
		if (work_stage == E_INIT_STAGE)
		{
			if (versionJudgement(protocol_data) == 1u)
			{
				if (protocol_data.FrameData.data[0] == 1u)
				{//返回确认正确
					work_stage = E_WORK_STAGE;
				}
				else
				{
					//需要增加错误处理
				}
			}
		}
		else if (work_stage == E_WORK_STAGE)
		{
			
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
			heart_time = 0;
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
