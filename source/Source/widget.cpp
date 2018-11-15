#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QTimer>

Widget::Widget(QWidget *parent,CBridge* bridge) :
    QWidget(parent),
	ui(new Ui::Widget),
	pb(bridge)
{
    ui->setupUi(this);
	scanButton = ui->pushButton;
	scanButton->setEnabled(false);
    //uart = new UartThread();
    //connect(uart,SIGNAL(readyRead(QByteArray)),this,SLOT(recSerialData()));
    //�������ҵ�����UI���ͨѶ
	connect(pb->business(), SIGNAL(signalSomethingComing(QByteArray)), SLOT(onComing(QByteArray)));
	//connect(this, SIGNAL(UiSometingDoing(QByteArray)), pb->business(), SLOT(onCommingAct(const QByteArray)));//���������ź����ӵ�ҵ��㣨����Ui������
    //initPort();
    initText();
	scanbroad_net_sta = 0;
	scan_arry.resize(LGJUpBound);
	scan_arry.fill(0xff);
	mesuring_flag = 0;//�����б�־λ
	measuring_timer = new QTimer(this);
	connect(measuring_timer, SIGNAL(timeout()), this, SLOT(timerOutSed()));
	/*�����õı���*/
	H_CrimpySelect = 0;			//������
	H_SynchronalSelect = 19700;	//ͬ����
	accu_area = 0;
	disp_accu = 0;
	oldFloat = 0;
	SheetArea = 0;
	carry_flag = E_CARRY_0;
	uint_flag = E_UINT_M2;
	/*��������*/
	QLabel* label = ui->label;
	QLabel* label2 = ui->label_2;
	QLabel* label3 = ui->label_3;
	label->setText("Ʒ��");
	label2->setText("��ɫ");
	label3->setText("�ȼ�");

	QLabel* label4 = ui->label_4;
	QLabel* label5 = ui->label_5;
	label4->setText("������λ");
	label5->setText("С��λ");

	QLabel* label6 = ui->label_6;
	label6->setText("С������");

	QLabel* label7 = ui->label_7;
	QLabel* label8 = ui->label_8;
	label7->setText("����Ա");
	label8->setText("�ͻ�");

	QLabel* label9 = ui->label_9;
	QLabel* label10 = ui->label_10;
	label9->setText("Ʒ�ֹ��");
	label10->setText("�����ϼ�");

	QLabel* label11 = ui->label_11;
	label11->setStyleSheet("color:blue;");

	QPushButton* button11 = ui->pushButton_11;
	QPushButton* button12 = ui->pushButton_12;
	button11->setText("ɾ��ĩ��");
	button12->setText("ĩС���ϲ�");

	QComboBox* box = ui->comboBox_4;
	box->addItem("ƽ��Ӣ��");
	box->addItem("ƽ����");
	box->addItem("ƽ���г�");

	QComboBox* box2 = ui->comboBox_5;
	box2->addItem("0.1");
	box2->addItem("0.01");
	box2->addItem("0.001");

	QLabel* label12 = ui->label_12;
	label12->setText("ƽ��Ӣ��");

	QGroupBox* groupBox16 = ui->groupBox_6;
	groupBox16->setStyleSheet("QGroupBox{background:rgb(255,192,255)}");

	QLabel* label13 = ui->label_13;
	label13->setVisible(false);		//���ڲ���
}

Widget::~Widget()
{
    delete ui;
}
/*ҵ�����Ϣ�Ĵ���*/
void Widget::onComing(QByteArray content)
{
	QLabel* label13 = ui->label_13;

	scan_arry.clear();
	scan_arry.fill(0xff);

	qDebug() <<"content"<< content;
	if (content.at(0) == 0x66)
	{
		switch (content.at(1))
		{
		case 3:
		{//����
			for (uint8_t i = 0; i < LGJUpBound; i++)
			{
				scan_arry[i] = content.at(2 + i);
			}
			scan_arry.append(content.at(2 + LGJUpBound));
			emit scanValsed(scan_arry);//�յ�������ת��
			break;
		}
		case 6:
		{
			accu_area = content.at(2);
			qDebug() << accu_area;
			oldFloat = AreaCoefficient(uint_flag);
			qDebug() << oldFloat;

			uint8_t NumSelect_Fractional = 0;
			switch (uint_flag)
			{
			case E_UINT_M2:
			{
				NumSelect_Fractional = 2;
				break;
			}
			case E_UINT_YC:
			{
				NumSelect_Fractional = 1;
				break;
			}
			case E_UINT_SC:
			{
				NumSelect_Fractional = 2;
				break;
			}
			default:
				break;
			}
			SheetArea = CarryUint((oldFloat * accu_area), NumSelect_Fractional);//NumSelect_FractionalС��λ��
			disp_accu = static_cast<float>(SheetArea);
			QLabel* dispAccu = ui->label_11;
			QString data = QString("%1").arg(disp_accu);
			dispAccu->setText(data);
			label13->setVisible(false);

		}
		default:
			break;
		}
	}
	if (content.indexOf("success") != -1)
	{
		//���ֳɹ�
		scanButton->setEnabled(true);
		scanbroad_net_sta = 1;
		label13->setVisible(false);

	}
	if (content.indexOf("disconnect") != -1)
	{
		scanButton->setEnabled(false);
		scanbroad_net_sta = 0;
		label13->setText("���ӶϿ�");
		label13->setVisible(true);
	}
	if (content.indexOf("mesuring") != -1)
	{
		//������
		mesuring_flag = 1;
		measuring_timer->start(1000);
		label13->setText("���ڲ���...");
		label13->setStyleSheet("color:red;");
		label13->setVisible(true);
	}
}


void Widget::initPort()
{
 /*   serial = new QSerialPort(this);

    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),this, &Widget::handleError);
    QObject::connect(serial,SIGNAL(readyRead()),this,SLOT(recSerialData()));
    serial->setPortName("COM3");
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setParity(QSerialPort::NoParity);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if(serial->open(QIODevice::ReadWrite))
    {
        qDebug()<<"open success";
    }
    else
    {
        qDebug()<<"open failed";
    }**/
}

void Widget::recSerialData()
{
  /*  QByteArray ba;
    ba = serial->readAll();
    qDebug()<<"receive info:"<<ba.toHex();
    //ui->textEdit->setText(ba);
    ba.clear();*/
}

void Widget::handleError(QSerialPort::SerialPortError error)
{
    /*
    if (error == QSerialPort::ResourceError)
    {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }*/
}


void Widget::closeSerialPort()
{
/*
    if (serial->isOpen())
        serial->close(); //�رմ���
*/
}

void Widget::initText()
{
    QStringList titles;
	QTableWidget *aTableWidget = ui->tableWidget;
	aTableWidget->setRowCount(30);
    aTableWidget->setColumnCount(3);
	titles << tr("��") << tr("����") << tr("С�����");
    aTableWidget->setHorizontalHeaderLabels(titles);
    QFont font = aTableWidget->horizontalHeader()->font();
    font.setBold(true);
    aTableWidget->horizontalHeader()->setFont(font);

}

void Widget::on_pushButton_clicked()
{
    ScanTest *dlg = new ScanTest(this);
	connect(dlg, SIGNAL(sedScanFrame()), pb->business(), SLOT(onScanSend()));
    connect(this,SIGNAL(scanValsed(QByteArray)),dlg,SLOT(showValue(QByteArray)));
    //emit scanValsed(scan_arry);
    dlg->exec();

}

void Widget::on_pushButton_MainSelect()
{
	MainSelect* mainSelect = new MainSelect(this);
	mainSelect->exec();
}

void Widget::on_pushButton_BatchPrint()
{
	BatchPrint* batchPrint = new BatchPrint(this);
	batchPrint->exec();
}

void Widget::on_pushButton_ParaSet()
{
	ParameterSet* paraSet = new ParameterSet(this);
	paraSet->exec();
}

void Widget::on_pushButton_OtherSet()
{
	OtherSet* otherset = new OtherSet(this);
	otherset->exec();
}

void Widget::mearing_timeout(void)
{
	static uint8_t cnt=0;

	cnt++;
	if (cnt >= MEASURING_TIMEOUT)
	{//������ ��ʱ
		cnt = 0;
		measuring_timer->stop();
		QLabel* label13 = ui->label_13;
		label13->setVisible(false);
	}
}

void Widget::timerOutSed(void)
{

}
/*����Ϊ��λ����ĺ���*/
uint16_t Widget::CarryUint(float oldFloat, uint8_t carrytype)
{
	uint32_t longtemp;
	uint32_t val;
	switch (carrytype)
	{
	case	1:			//carrytype=1Ϊ0.25��
		oldFloat = (oldFloat * 4) + 0.5;
		longtemp = float_to_int(oldFloat);
		val = (250 * longtemp);
		break;
	case	2:			//carrytype=2Ϊ0.1
		oldFloat = (oldFloat * 10) + 0.5;
		longtemp = static_cast<uint32_t>(float_to_int(oldFloat));
		val = (100 * longtemp);
		break;
	case	3:			//carrytype=3Ϊ0.01
		oldFloat = (oldFloat * 100) + 0.5;
		longtemp = float_to_int(oldFloat);
		val = (10 * longtemp);
		break;
	case	4:			//carrytype=4Ϊ0.001
		oldFloat = (oldFloat * 1000) + 0.5;
		longtemp = float_to_int(oldFloat);
		val = (longtemp);
		break;
	default:
		break;
	}
	return val;
}

float Widget::AreaCoefficient(eUintType flag)
{
	uint16_t CrimpySelect = H_CrimpySelect;
	uint16_t SynchronalSelect = H_SynchronalSelect;
	float ret = 0;
	switch (flag)
	{
	case E_UINT_M2:
	{
		ret = 0.00001* (CrimpySelect + 1000) * (100 + SynchronalSelect*0.01)*0.0001;//ƽ����
		break;
	}
	case E_UINT_YC:
	{
		ret = 0.00001* (CrimpySelect + 1000) * (100 + SynchronalSelect*0.01)*0.001076;//ƽ��Ӣ��
		break;
	}
	case E_UINT_SC:
	{
		ret = 0.00001* (CrimpySelect + 1000) * (100 + SynchronalSelect*0.01)*0.0009;//ƽ���г� 
		break;
	}
	default:
		break;
	}
	return ret;
}

int Widget::float_to_int(float f)
{
	int *p = (int*)&f;
	//����ָ������ڴ��ǰ��ջ����ͽ��еģ����Ƚ���ǿת���ʸ�����f
	int temp = *p;
	//���м��������f��ֵ
	//��ȡ����λ
	int sign = -1;
	if ((temp & 0x80000000) == 0)
	{
		sign = 1;
	}
	//�������ʽ�н�����λ�����㣬��ʵ������ȡָ��λ��ֵ��ǰ���Ѿ������ˣ�����λΪ0����Ϊ��ֵ����Ϊ1����Ϊ��ֵ��

	//��ȡָ��λ
	int exp;
	exp = (((temp >> 23) & 0x0ff)) - 127;
	//�����ڴ洢��ʱ��ָ��λ����ƫ�ó���   �����൱���Ǽ���ָ��λ������� ������ȻҪ��ȥ

	//��ȡβ��λ
	int tail;
	tail = ((temp & 0x007fffff) | 0x00800000);
	/*��֮ǰ��ȡָ��λ�ͷ���λ�ķ�����һ�µģ�Ҫ��ȡ�ļ�λ���ڶ�Ӧ��λ������1����Ϳ�����
	��������Ҫ��ȡ���ǳ�����λ��ָ��λ���ƾ�λ������23����β��Ϊ��ֻ��Ҫ�ڶ�Ӧ��Ϊ��1����*/
	/*���ǵ��ڴ洢β����ʱ����Ϊ����һ�������ȵĶ��������Կ�ѧ��������ʾʱ����һλ����1��
	���Դ洢��ʱ�䣬Ϊ���ܹ���߾��ȣ�ʡ���˸�λ�������������ڽ�����������̣���������Ҫ��
	Ҫ���л�1λ����*/

	int res = (tail >> (23 - exp));

	return res * sign;

}


/*���ʣ��ڷ���֡�����ʱ��᲻�ᶨʱ������Ȼ����һ֡������
һ֡���
�²⣺���� ��Ϊû����ռ����ʱ������������е�ʱ�򴥷�*/
