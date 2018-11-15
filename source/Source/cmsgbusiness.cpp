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
	scan_data.resize(LGJUpBound+5u);//5ΪԤ��
	scan_data.clear();
	sedFrame = { 0,0,static_cast<eFrameType>(0),0,0,0,0 };
	sedArry = "cn";
	rec_flag = 0;
	//lenth = 0;	//Ҫ���յ�����֡��
	accu_area = 0;
}
CMsgBusiness::~CMsgBusiness()
{
	
}
/*�����յ�����*/
void CMsgBusiness::onCommingMsg(const QByteArray msg)
{   //������Զ��յ���msg���д���
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

/*����֡����  �˴��Ĳ������ظ� ��ʱ����*/
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
		//����֡����������
		break;
	}
	case E_FRAME_HANDSHAKE:
	{
		sedFrame.len = 0x04u;
		//����֡����������
		break;
	}
	case E_FRAME_DETECTION:
	{
		sedFrame.len = 0x05u;
		sedFrame.FrameData.data[0] = data.data[0];	//�����ֽ� 1
		break;
	}
	case E_FRAME_MEASURING:
	{
		sedFrame.len = 0x05u;
		sedFrame.FrameData.data[0] = data.data[0];	//�ظ��ֽ� 2
		break;
	}
	default:
		break;
	}
	sedFrame.FrameData.len = sedFrame.len - FRAME_LEN_DIFF;

}
/*֡�������*/
void CMsgBusiness::sedFramePack(sFrame frame)
{
	
	sedArry.clear();
	uint16_t len = frame.len + 4u;
	if (len > 500u)	//����֡��С����
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
/* ����1���ɹ��յ����� 0������ʧ��*/
uint8_t CMsgBusiness::dataDeal(QByteArray msg)
{
	//�˴��������ݴ���
	int16_t idx = -1;
	uint16_t len = 0,hard = 0;
	uint8_t ret = 0;
	int16_t i;

	if (msg.size() > 200)	//����һ֡�����ܴ���200�ֽ�
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
				ret = 1;//֡У��ͨ��
			}
			if (len < 4)
			{
				ret = 0;//��С֡����
			}
		}
		if (ret == 1)
		{//֡�������
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
/*���ղ���
�յ����� ������*/
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
				{//���ֳɹ�
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
					//��Ҫ���Ӵ�����
				}
			}
		}
		else if (work_stage == E_WORK_STAGE)
		{
			//����״̬�յ����ֲ�����
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
			//�յ�����
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
				if (frame.FrameData.data[0] == 0x02u)//�����ֽ�Ϊ�ظ�
				{
					if (frame.FrameData.len == LGJUpBound+1)//����У��
					{
						//qDebug()<<"E_FRAME_DETECTION";
						scan_data.clear();
						scan_data.append(0x66);//��һ���ֽ�Ϊ66 Ϊ����
						scan_data.append(0x03);//�ڶ����ֽ�Ϊ3	Ϊ����֡
						uint8_t flag_all_temp = 0;//ȫ�ձ�־λ
						uint8_t temp = 0;//����
						for (uint8_t i = 1; i < LGJUpBound + 1; i++)
						{
							scan_data.append(frame.FrameData.data[i]);
							/*�������Ϊ�ж��Ƿ�Ϊȫ��*/
							if (flag_all_temp == 0)//0 Ϊȫ�� 1Ϊ��ȫ��
							{
								if (i < LGJUpBound)
								{//��Ϊ���һ��
									if (frame.FrameData.data[i] != 0)
									{
										flag_all_temp = 1;
									}
								}
								else
								{//���һ�� ��Ҫ����
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
				//Ӳ���汾����
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
				if (frame.FrameData.data[0] == 0x01u)//�����ֽ�Ϊ����
				{
					rec_flag = 2;
					//�Ϸ� ��ʾ������
					dismsg.clear();
					dismsg.append("mesuring");
					emit signalSomethingComing(dismsg);
					//�·� �ظ�
					sFrameData seddata = {1,2};//Ҫ���͵�����

					sedDeal(E_FRAME_MEASURING, seddata);//���ͽṹ�帳ֵ����
					sedFramePack(sedFrame);//�ṹ����������֡����
					emit signalToSend(sedArry);// �����ź�
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
				if (frame.FrameData.data[0] == 0x01u)//�����ֽ�Ϊ����
				{
					uint16_t len=0;

					len |= 0xff00&(static_cast<uint16_t>(frame.FrameData.data[1]) << 8);
					len |= 0x00ff&(static_cast<uint16_t>(frame.FrameData.data[2]));
					qDebug() << len;
					if (len < 1 || len >500)
					{
						qDebug() << "��������֡�յ�����ֵ�����";
						//�·� �ظ�
						sFrameData seddata = { 2,2,2 };//Ҫ���͵�����

						sedDeal(E_FRAME_MEASURING, seddata);//���ͽṹ�帳ֵ����
						sedFramePack(sedFrame);//�ṹ����������֡����
						emit signalToSend(sedArry);// �����ź�
					}
					else
					{
						//lenth = len;
						//�Ϸ� ��ʾ������
						dismsg.clear();
						dismsg.append("end measure");
						emit signalSomethingComing(dismsg);
						//�·� �ظ�
						sFrameData seddata = { 2,2,1 };//Ҫ���͵�����

						sedDeal(E_FRAME_MEASURING, seddata);//���ͽṹ�帳ֵ����
						sedFramePack(sedFrame);//�ṹ����������֡����
						emit signalToSend(sedArry);// �����ź�
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
				if (frame.FrameData.data[0] == 0x01u)//�����ֽ�Ϊ����
				{
					accu_area |= static_cast<uint16_t>(frame.FrameData.data[1]) << 8;
					accu_area |= static_cast<uint16_t>(frame.FrameData.data[2]);
					rec_flag = 3;
					//�Ϸ� ��ʾ������
					dismsg.clear();
					dismsg.append(0x66);
					dismsg.append(6);
					dismsg.append(accu_area);
					emit signalSomethingComing(dismsg);
					/*��ʱʹ���� ֡�� �����ݵķ�ʽ ������Ҫȥд��ʾ�ͽ��մ���Ĳ���11/11*/
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
	/*��ʱֻ�ж�Ӳ���汾*/
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
//����ɨ��
void CMsgBusiness::onScanSend(void)
{
	sFrameData seddata = { 1,1 };//Ҫ���͵�����

	sedDeal(E_FRAME_DETECTION, seddata);//���ͽṹ�帳ֵ����
	sedFramePack(sedFrame);//�ṹ����������֡����
	emit signalToSend(sedArry);// �����ź�
}
/*�������Ͷ�ʱ���ﺯ��*/
void CMsgBusiness::timerOutHeart(void)
{
	sFrameData seddata = { 0,0 };//Ҫ���͵�����

	if (heart_lost_cnt >= HEART_LOST_CNT)
	{//�ղ�����������
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
		sedDeal(E_FRAME_HEARTBEAT, seddata);//���ͽṹ�帳ֵ����
		sedFramePack(sedFrame);//�ṹ����������֡����
		emit signalToSend(sedArry);// �����ź�
		heart_lost_cnt++;
	}
}
/*���ַ��Ͷ�ʱ�����ﺯ��*/
void CMsgBusiness::timerOuthands(void)
{
	sFrameData seddata = { 0,0 };//Ҫ���͵�����

	sedDeal(E_FRAME_HANDSHAKE, seddata);//���ͽṹ�帳ֵ����
	sedFramePack(sedFrame);//�ṹ����������֡����
	emit signalToSend(sedArry);// �����ź�
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

