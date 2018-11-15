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
	rec_flag = 0;
	//lenth = 0;	//要接收的数据帧数
	accu_area = 0;
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

/*发送帧处理  此处的参数有重复 暂时保留*/
void CMsgBusiness::sedDeal(eFrameType type,sFrameData data)
{
	sedFrame.header = FRAME_HEX_HEADER;
	sedFrame.version = SOFEWAVE_VERSION;
	sedFrame.hard = LGJType;
	sedFrame.tail = FRAME_HEX_TAIL;
	sedFrame.type = type;

	switch (type)
	{
	case E_FRAME_HEARTBEAT:
	{
		sedFrame.len = 0x04u;
		//心跳帧不包含数据
		break;
	}
	case E_FRAME_HANDSHAKE:
	{
		sedFrame.len = 0x04u;
		//握手帧不包含数据
		break;
	}
	case E_FRAME_DETECTION:
	{
		sedFrame.len = 0x05u;
		sedFrame.FrameData.data[0] = data.data[0];	//请求字节 1
		break;
	}
	case E_FRAME_MEASURING:
	{
		sedFrame.len = 0x05u;
		sedFrame.FrameData.data[0] = data.data[0];	//回复字节 2
		break;
	}
	default:
		break;
	}
	sedFrame.FrameData.len = sedFrame.len - FRAME_LEN_DIFF;

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
		//qDebug() << msg;
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
						//qDebug()<<"E_FRAME_DETECTION";
						scan_data.clear();
						scan_data.append(0x66);//第一个字节为66 为数据
						scan_data.append(0x03);//第二个字节为3	为各类帧
						uint8_t flag_all_temp = 0;//全空标志位
						uint8_t temp = 0;//掩码
						for (uint8_t i = 1; i < LGJUpBound + 1; i++)
						{
							scan_data.append(frame.FrameData.data[i]);
							/*下面代码为判断是否为全空*/
							if (flag_all_temp == 0)//0 为全空 1为不全空
							{
								if (i < LGJUpBound)
								{//不为最后一组
									if (frame.FrameData.data[i] != 0)
									{
										flag_all_temp = 1;
									}
								}
								else
								{//最后一组 需要掩码
									if (frame.FrameData.data[i] != 0)
									{
										if (LGJLightNum <= 8)
										{
											temp = frame.FrameData.data[i];
											temp = temp << LGJLightNum;
											temp &= 0x00ff;
											if (temp != 0)
											{
												flag_all_temp = 1;
											}
										}
										else
										{
											flag_all_temp = 1;
										}
									}
								}
							}
						}
						scan_data.append(flag_all_temp);
						emit signalSomethingComing(scan_data);
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
	case E_FRAME_MEASURING:
	{
		if (work_stage == E_WORK_STAGE)
		{
			if (versionJudgement(frame) == 1u)
			{
				if (frame.FrameData.data[0] == 0x01u)//请求字节为请求
				{
					rec_flag = 2;
					//上发 显示测量中
					dismsg.clear();
					dismsg.append("mesuring");
					emit signalSomethingComing(dismsg);
					//下发 回复
					sFrameData seddata = {1,2};//要发送的数据

					sedDeal(E_FRAME_MEASURING, seddata);//发送结构体赋值函数
					sedFramePack(sedFrame);//结构体打包成数组帧函数
					emit signalToSend(sedArry);// 发送信号
				}
			}
		}
		break;
	}
	case E_FRAME_END_MEASURE:
	{
		if (work_stage == E_WORK_STAGE)
		{
			if (versionJudgement(frame) == 1u)
			{
				if (frame.FrameData.data[0] == 0x01u)//请求字节为请求
				{
					uint16_t len=0;

					len |= 0xff00&(static_cast<uint16_t>(frame.FrameData.data[1]) << 8);
					len |= 0x00ff&(static_cast<uint16_t>(frame.FrameData.data[2]));
					qDebug() << len;
					if (len < 1 || len >500)
					{
						qDebug() << "测量结束帧收到的数值不达标";
						//下发 回复
						sFrameData seddata = { 2,2,2 };//要发送的数据

						sedDeal(E_FRAME_MEASURING, seddata);//发送结构体赋值函数
						sedFramePack(sedFrame);//结构体打包成数组帧函数
						emit signalToSend(sedArry);// 发送信号
					}
					else
					{
						//lenth = len;
						//上发 显示测量中
						dismsg.clear();
						dismsg.append("end measure");
						emit signalSomethingComing(dismsg);
						//下发 回复
						sFrameData seddata = { 2,2,1 };//要发送的数据

						sedDeal(E_FRAME_MEASURING, seddata);//发送结构体赋值函数
						sedFramePack(sedFrame);//结构体打包成数组帧函数
						emit signalToSend(sedArry);// 发送信号
					}
				}
			}
		}
		break;
	}
	case E_FRAME_DATA_MEASURE:
	{
		if (work_stage == E_WORK_STAGE)
		{
			if (versionJudgement(frame) == 1u)
			{
				if (frame.FrameData.data[0] == 0x01u)//请求字节为请求
				{
					accu_area |= static_cast<uint16_t>(frame.FrameData.data[1]) << 8;
					accu_area |= static_cast<uint16_t>(frame.FrameData.data[2]);
					rec_flag = 3;
					//上发 显示测量中
					dismsg.clear();
					dismsg.append(0x66);
					dismsg.append(6);
					dismsg.append(accu_area);
					emit signalSomethingComing(dismsg);
					/*暂时使用了 帧号 加数据的方式 后面需要去写显示和接收处理的部分11/11*/
				}
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
//发送扫描
void CMsgBusiness::onScanSend(void)
{
	sFrameData seddata = { 1,1 };//要发送的数据

	sedDeal(E_FRAME_DETECTION, seddata);//发送结构体赋值函数
	sedFramePack(sedFrame);//结构体打包成数组帧函数
	emit signalToSend(sedArry);// 发送信号
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

