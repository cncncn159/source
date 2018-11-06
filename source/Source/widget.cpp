#include "widget.h"
#include "ui_widget.h"
#include <QDebug>


Widget::Widget(QWidget *parent,CBridge* bridge) :
    QWidget(parent),
	ui(new Ui::Widget),
	pb(bridge)
{
    ui->setupUi(this);
    //uart = new UartThread();
    //connect(uart,SIGNAL(readyRead(QByteArray)),this,SLOT(recSerialData()));
    //这里完成业务层与UI层的通讯
	connect(pb->business(), SIGNAL(signalSomethingComing(QByteArray)), SLOT(onComing(QByteArray)));
	//connect(this, SIGNAL(UiSometingDoing(QByteArray)), pb->business(), SLOT(onCommingAct(const QByteArray)));//这里增加信号连接到业务层（发送Ui操作）
    //initPort();
    initText();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onComing(QByteArray content)
{
	qDebug() <<"content"<< content;
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
    //int value = 0x7777;
    QByteArray array("cncncncncncn");

    ScanTest *dlg = new ScanTest(this);
    connect(this,SIGNAL(dlgReturn(QByteArray)),dlg,SLOT(showValue(QByteArray)));
    //emit dlgReturn(array);
    dlg->exec();

}
