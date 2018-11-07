#include "widget.h"
#include "ui_widget.h"
#include <QDebug>


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
}

Widget::~Widget()
{
    delete ui;
}
/*ҵ�����Ϣ�Ĵ���*/
void Widget::onComing(QByteArray content)
{
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
		default:
			break;
		}
	}
	if (content.indexOf("success") != -1)
	{
		//���ֳɹ�
		scanButton->setEnabled(true);
		scanbroad_net_sta = 1;
	}
	if (content.indexOf("disconnect") != -1)
	{
		scanButton->setEnabled(false);
		scanbroad_net_sta = 0;
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
