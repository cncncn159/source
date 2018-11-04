#include "scantest.h"
#include "ui_scantest.h"
#include <QDebug>

ScanTest::ScanTest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScanTest)
{
    ui->setupUi(this);
    QLineEdit* lineedit1 = ui->lineEdit;
    QLineEdit* lineedit2 = ui->lineEdit_2;
    QLineEdit* lineedit3 = ui->lineEdit_3;
    QLineEdit* lineedit4 = ui->lineEdit_4;
    QLineEdit* lineedit5 = ui->lineEdit_5;
    QLineEdit* lineedit6 = ui->lineEdit_6;
    QLineEdit* lineedit7 = ui->lineEdit_7;
    QLineEdit* lineedit8 = ui->lineEdit_8;

    lineedit1->setMaxLength(20);
    lineedit1->setFont(QFont("Timers",28,QFont::Bold));
    lineedit1->setText(" 11111111 11111111 ");

    lineedit2->setMaxLength(20);
    lineedit2->setFont(QFont("Timers",28,QFont::Bold));
    lineedit2->setText(" 00000000 00000000 ");

    lineedit3->setMaxLength(20);
    lineedit3->setFont(QFont("Timers",28,QFont::Bold));
    lineedit3->setText(" ******** ******** ");

    lineedit4->setMaxLength(20);
    lineedit4->setFont(QFont("Timers",28,QFont::Bold));
    lineedit4->setText(" 00000000 00000000 ");

    lineedit5->setMaxLength(20);
    lineedit5->setFont(QFont("Timers",28,QFont::Bold));
    lineedit5->setText(" 00000000 00000000 ");

    lineedit6->setMaxLength(20);
    lineedit6->setFont(QFont("Timers",28,QFont::Bold));
    lineedit6->setText(" 00000000 00000000 ");

    lineedit7->setMaxLength(20);
    lineedit7->setFont(QFont("Timers",28,QFont::Bold));
    lineedit7->setText(" 00000000 00000000 ");

    lineedit8->setMaxLength(20);
    lineedit8->setFont(QFont("Timers",28,QFont::Bold));
    lineedit8->setText(" 00000000 00000000 ");

}

ScanTest::~ScanTest()
{
    delete ui;
}

void ScanTest::showValue(QByteArray value)
{
    qDebug()<<"scantest show: "<<value;
    //QLineEdit* lineedit1 = ui->lineEdit;
    //QString info = QString("0b%1").arg(value,0,2);
    //lineedit1->setText(info);
}
