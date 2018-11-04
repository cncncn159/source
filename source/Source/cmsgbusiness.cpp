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
	if (event->timerId() == timer_id)
	{
		if (heart_time <= 0xffffu)
		{
			heart_time++;
		}
		if (heart_time >= HEART_WAIT_TIME)
		{
			//发送心跳
		}
	}
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
			//do nothing
		}
		else if (work_stage == E_WORK_STAGE)
		{
			heart_ret_time = 0;
		}
		break;
	}
	default:
		break;
	}
	return ret;
}

void CMsgBusiness::onCommingAct(const QByteArray)
{

}
