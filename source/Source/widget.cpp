#include "widget.h"
#include "ui_widget.h"
#include <QDebug>


Widget::Widget(QWidget *parent,CBridge* bridge) :
    QWidget(parent),
	ui(new Ui::Widget),
	pb(bridge)
{
    ui->setupUi(this);
    //这里完成业务层与UI层的通讯
	connect(pb->business(), SIGNAL(signalSomethingComing(QByteArray)), SLOT(onComing(QByteArray)));

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

}

void Widget::recSerialData()
{

}

void Widget::handleError(QSerialPort::SerialPortError error)
{

}


void Widget::closeSerialPort()
{

}

void Widget::initText()
{
    QStringList titles;
	QTableWidget *aTableWidget = ui->tableWidget;
	aTableWidget->setRowCount(30);
    aTableWidget->setColumnCount(3);
	titles << tr("bao") << tr("zhangshu") << tr("xiaojimianji");
    aTableWidget->setHorizontalHeaderLabels(titles);
    //QFont font = aTableWidget->horizontalHeader()->font();
    //font.setBold(true);
    //aTableWidget->horizontalHeader()->setFont(font);

}

void Widget::on_pushButton_clicked()
{
    QByteArray array("cncncncncncn");

    ScanTest *dlg = new ScanTest(this);
    connect(this,SIGNAL(dlgReturn(QByteArray)),dlg,SLOT(showValue(QByteArray)));
    dlg->exec();

}
