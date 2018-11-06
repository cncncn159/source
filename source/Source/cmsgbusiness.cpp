#include "cmsgbusiness.h"


CMsgBusiness::CMsgBusiness(QObject *parent) : QObject(parent)
{
	timer_heart = new QTimer(this);
	timer_hands = new QTimer(this);
	connect(timer_heart, SIGNAL(timeout()), this, SLOT(timerOutHeart()));
	connect(timer_hands, SIGNAL(timeout()), this, SLOT(timerOuthands()));
	timer_hands->start(HEART_POLL_TIME);
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

/*����֡����*/
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
		//����֡����������
		break;
	}
	default:
		break;
	}
}
/*֡�������*/
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
/* ����1���ɹ��յ����� 0������ʧ��*/
uint8_t CMsgBusiness::dataDeal(QByteArray msg)
{
	//�˴��������ݴ���
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

	if (msg.size() >= idx + len + 3u)
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
/*�յ����� ������*/
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
			if (versionJudgement(protocol_data) == 1u)
			{
				if (protocol_data.FrameData.data[0] == 1u)
				{//���ֳɹ�
					dismsg.resize(20);
					dismsg.append("connect success");
					work_stage = E_WORK_STAGE;
					emit signalSomethingComing(dismsg);
					timer_heart->start(HANDS_POLL_TIME);
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
/*�������Ͷ�ʱ���ﺯ��*/
void CMsgBusiness::timerOutHeart(void)
{
	sFrameData seddata = { 0,0 };//Ҫ���͵�����

	if (heart_lost_cnt >= HEART_LOST_CNT)
	{
		work_stage = E_INIT_STAGE;
		timer_heart->stop();
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
