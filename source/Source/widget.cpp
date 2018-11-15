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
    //这里完成业务层与UI层的通讯
	connect(pb->business(), SIGNAL(signalSomethingComing(QByteArray)), SLOT(onComing(QByteArray)));
	//connect(this, SIGNAL(UiSometingDoing(QByteArray)), pb->business(), SLOT(onCommingAct(const QByteArray)));//这里增加信号连接到业务层（发送Ui操作）
    //initPort();
    initText();
	scanbroad_net_sta = 0;
	scan_arry.resize(LGJUpBound);
	scan_arry.fill(0xff);
	mesuring_flag = 0;//测量中标志位
	measuring_timer = new QTimer(this);
	connect(measuring_timer, SIGNAL(timeout()), this, SLOT(timerOutSed()));
	/*换算用的变量*/
	H_CrimpySelect = 0;			//皱缩率
	H_SynchronalSelect = 19700;	//同步率
	accu_area = 0;
	disp_accu = 0;
	oldFloat = 0;
	SheetArea = 0;
	carry_flag = E_CARRY_0;
	uint_flag = E_UINT_M2;
	/*界面设置*/
	QLabel* label = ui->label;
	QLabel* label2 = ui->label_2;
	QLabel* label3 = ui->label_3;
	label->setText("品种");
	label2->setText("颜色");
	label3->setText("等级");

	QLabel* label4 = ui->label_4;
	QLabel* label5 = ui->label_5;
	label4->setText("计量单位");
	label5->setText("小数位");

	QLabel* label6 = ui->label_6;
	label6->setText("小计张数");

	QLabel* label7 = ui->label_7;
	QLabel* label8 = ui->label_8;
	label7->setText("操作员");
	label8->setText("客户");

	QLabel* label9 = ui->label_9;
	QLabel* label10 = ui->label_10;
	label9->setText("品种规格");
	label10->setText("本批合计");

	QLabel* label11 = ui->label_11;
	label11->setStyleSheet("color:blue;");

	QPushButton* button11 = ui->pushButton_11;
	QPushButton* button12 = ui->pushButton_12;
	button11->setText("删除末张");
	button12->setText("末小包合并");

	QComboBox* box = ui->comboBox_4;
	box->addItem("平方英尺");
	box->addItem("平方米");
	box->addItem("平方市尺");

	QComboBox* box2 = ui->comboBox_5;
	box2->addItem("0.1");
	box2->addItem("0.01");
	box2->addItem("0.001");

	QLabel* label12 = ui->label_12;
	label12->setText("平方英尺");

	QGroupBox* groupBox16 = ui->groupBox_6;
	groupBox16->setStyleSheet("QGroupBox{background:rgb(255,192,255)}");

	QLabel* label13 = ui->label_13;
	label13->setVisible(false);		//正在测量
}

Widget::~Widget()
{
    delete ui;
}
/*业务层消息的传递*/
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
		{//测量
			for (uint8_t i = 0; i < LGJUpBound; i++)
			{
				scan_arry[i] = content.at(2 + i);
			}
			scan_arry.append(content.at(2 + LGJUpBound));
			emit scanValsed(scan_arry);//收到后立马转发
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
			SheetArea = CarryUint((oldFloat * accu_area), NumSelect_Fractional);//NumSelect_Fractional小数位数
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
		//握手成功
		scanButton->setEnabled(true);
		scanbroad_net_sta = 1;
		label13->setVisible(false);

	}
	if (content.indexOf("disconnect") != -1)
	{
		scanButton->setEnabled(false);
		scanbroad_net_sta = 0;
		label13->setText("连接断开");
		label13->setVisible(true);
	}
	if (content.indexOf("mesuring") != -1)
	{
		//测量中
		mesuring_flag = 1;
		measuring_timer->start(1000);
		label13->setText("正在测量...");
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
        serial->close(); //关闭串口
*/
}

void Widget::initText()
{
    QStringList titles;
	QTableWidget *aTableWidget = ui->tableWidget;
	aTableWidget->setRowCount(30);
    aTableWidget->setColumnCount(3);
	titles << tr("包") << tr("张数") << tr("小计面积");
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
	{//测量中 超时
		cnt = 0;
		measuring_timer->stop();
		QLabel* label13 = ui->label_13;
		label13->setVisible(false);
	}
}

void Widget::timerOutSed(void)
{

}
/*下面为单位换算的函数*/
uint16_t Widget::CarryUint(float oldFloat, uint8_t carrytype)
{
	uint32_t longtemp;
	uint32_t val;
	switch (carrytype)
	{
	case	1:			//carrytype=1为0.25进
		oldFloat = (oldFloat * 4) + 0.5;
		longtemp = float_to_int(oldFloat);
		val = (250 * longtemp);
		break;
	case	2:			//carrytype=2为0.1
		oldFloat = (oldFloat * 10) + 0.5;
		longtemp = static_cast<uint32_t>(float_to_int(oldFloat));
		val = (100 * longtemp);
		break;
	case	3:			//carrytype=3为0.01
		oldFloat = (oldFloat * 100) + 0.5;
		longtemp = float_to_int(oldFloat);
		val = (10 * longtemp);
		break;
	case	4:			//carrytype=4为0.001
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
		ret = 0.00001* (CrimpySelect + 1000) * (100 + SynchronalSelect*0.01)*0.0001;//平方米
		break;
	}
	case E_UINT_YC:
	{
		ret = 0.00001* (CrimpySelect + 1000) * (100 + SynchronalSelect*0.01)*0.001076;//平方英尺
		break;
	}
	case E_UINT_SC:
	{
		ret = 0.00001* (CrimpySelect + 1000) * (100 + SynchronalSelect*0.01)*0.0009;//平方市尺 
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
	//由于指针访问内存是按照基类型进行的，首先进行强转访问浮点数f
	int temp = *p;
	//用中间变量接受f的值
	//提取符号位
	int sign = -1;
	if ((temp & 0x80000000) == 0)
	{
		sign = 1;
	}
	//条件表达式中进行了位与运算，其实就是提取指数位的值。前面已经讲过了，符号位为0，其为正值。若为1，其为负值。

	//提取指数位
	int exp;
	exp = (((temp >> 23) & 0x0ff)) - 127;
	//由于在存储的时候指数位加了偏置常数   现在相当于是计算指数位的逆过程 所以自然要减去

	//提取尾数位
	int tail;
	tail = ((temp & 0x007fffff) | 0x00800000);
	/*和之前提取指数位和符号位的方法是一致的，要提取哪几位，在对应的位进行与1运算就可以了
	现在我们要提取的是除符号位和指数位共计九位的其他23，即尾数为。只需要在对应的为与1即可*/
	/*还记得在存储尾数的时候，因为任意一个单精度的二进制数以科学记数法表示时，第一位都是1，
	所以存储的时间，为了能够提高精度，省略了改位。但是现在是在进行他的逆过程，所以我们要在
	要进行或1位运算*/

	int res = (tail >> (23 - exp));

	return res * sign;

}


/*疑问：在发送帧打包的时候会不会定时器到，然后发送一帧，把上
一帧冲掉
猜测：不会 因为没有抢占，定时器会堵塞到空闲的时候触发*/
