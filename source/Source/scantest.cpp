#include "scantest.h"
#include "ui_scantest.h"
#include <QDebug>

ScanTest::ScanTest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScanTest)
{
    ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	scan_data.resize(20);//最大20组
	scan_data.fill(0xff);
	timer_refresh = new QTimer(this);
	connect(timer_refresh, SIGNAL(timeout()), this, SLOT(timerOutSed()));
	timer_refresh->start(SCAN_REFRESH_INTERVAL);

    QLineEdit* lineedit1 = ui->lineEdit;
    QLineEdit* lineedit2 = ui->lineEdit_2;
    QLineEdit* lineedit3 = ui->lineEdit_3;
    QLineEdit* lineedit4 = ui->lineEdit_4;
    QLineEdit* lineedit5 = ui->lineEdit_5;
    QLineEdit* lineedit6 = ui->lineEdit_6;
    QLineEdit* lineedit7 = ui->lineEdit_7;
    QLineEdit* lineedit8 = ui->lineEdit_8;
	QLineEdit* lineedit9 = ui->lineEdit_9;
	QLineEdit* lineedit10 = ui->lineEdit_10;

	QCheckBox* checkbox1 = ui->checkBox;
	checkbox1->setCheckable(0);
	checkbox1->setText(tr("初始化失败"));
	
    lineedit1->setMaxLength(20);
    lineedit1->setFont(QFont("Timers",28,QFont::Bold));
	QString info = QString("  %1 %2").arg(0xff, 0, 2).arg(0xff, 0, 2);
    lineedit1->setText(info);

    lineedit2->setMaxLength(20);
    lineedit2->setFont(QFont("Timers",28,QFont::Bold));
    lineedit2->setText(info);

    lineedit3->setMaxLength(20);
    lineedit3->setFont(QFont("Timers",28,QFont::Bold));
    lineedit3->setText(info);

    lineedit4->setMaxLength(20);
    lineedit4->setFont(QFont("Timers",28,QFont::Bold));
    lineedit4->setText(info);

    lineedit5->setMaxLength(20);
    lineedit5->setFont(QFont("Timers",28,QFont::Bold));
    lineedit5->setText(info);

    lineedit6->setMaxLength(20);
    lineedit6->setFont(QFont("Timers",28,QFont::Bold));
    lineedit6->setText(info);

    lineedit7->setMaxLength(20);
    lineedit7->setFont(QFont("Timers",28,QFont::Bold));
    lineedit7->setText(info);

    lineedit8->setMaxLength(20);
    lineedit8->setFont(QFont("Timers",28,QFont::Bold));
    lineedit8->setText(info);

	lineedit9->setMaxLength(20);
	lineedit9->setFont(QFont("Timers", 28, QFont::Bold));
	lineedit9->setText(info);

	lineedit10->setMaxLength(20);
	lineedit10->setFont(QFont("Timers", 28, QFont::Bold));
	lineedit10->setText(info);
}

ScanTest::~ScanTest()
{
	timer_refresh->stop();
    delete ui;
}

void ScanTest::timerOutSed(void)
{
	emit sedScanFrame();
}

void ScanTest::showValue(QByteArray value)
{
    qDebug()<<"scantest show: "<<value;
	QCheckBox* checkbox1 = ui->checkBox;
	
	if (value.size() > 22)
	{
		return;
	}
	for (uint8_t i = 0; i < (value.size()-1); i++)
	{
		scan_data[i] = value.at(i);
	}

	if (value.at(value.size()-1) == 0)
	{
		checkbox1->setChecked(1);
		checkbox1->setText(tr("初始化成功"));
	}
	else
	{
		checkbox1->setChecked(0);
		checkbox1->setText(tr("初始化失败"));

	}
    QLineEdit* lineedit1 = ui->lineEdit;
	QString info = QString("  %1 %2").arg(static_cast<uint8_t>(scan_data.at(0)), 0, 2).arg(static_cast<uint8_t>(scan_data.at(1)), 0, 2);
    lineedit1->setText(info);

	QLineEdit* lineedit2 = ui->lineEdit_2;
	QString info2 = QString("  %1 %2").arg(static_cast<uint8_t>(scan_data.at(2)), 0, 2).arg(static_cast<uint8_t>(scan_data.at(3)), 0, 2);
	lineedit2->setText(info2);

	QLineEdit* lineedit3 = ui->lineEdit_3;
	QString info3 = QString("  %1 %2").arg(static_cast<uint8_t>(scan_data.at(4)), 0, 2).arg(static_cast<uint8_t>(scan_data.at(5)), 0, 2);
	lineedit3->setText(info3);

	QLineEdit* lineedit4 = ui->lineEdit_4;
	QString info4 = QString("  %1 %2").arg(static_cast<uint8_t>(scan_data.at(6)), 0, 2).arg(static_cast<uint8_t>(scan_data.at(7)), 0, 2);
	lineedit4->setText(info4);

	QLineEdit* lineedit5 = ui->lineEdit_5;
	QString info5 = QString("  %1 %2").arg(static_cast<uint8_t>(scan_data.at(8)), 0, 2).arg(static_cast<uint8_t>(scan_data.at(9)), 0, 2);
	lineedit5->setText(info5);

	QLineEdit* lineedit6 = ui->lineEdit_6;
	QString info6 = QString("  %1 %2").arg(static_cast<uint8_t>(scan_data.at(10)), 0, 2).arg(static_cast<uint8_t>(scan_data.at(11)), 0, 2);
	lineedit6->setText(info6);

	QLineEdit* lineedit7 = ui->lineEdit_7;
	QString info7 = QString("  %1 %2").arg(static_cast<uint8_t>(scan_data.at(12)), 0, 2).arg(static_cast<uint8_t>(scan_data.at(13)), 0, 2);
	lineedit7->setText(info7);

	QLineEdit* lineedit8 = ui->lineEdit_8;
	QString info8 = QString("  %1 %2").arg(static_cast<uint8_t>(scan_data.at(14)), 0, 2).arg(static_cast<uint8_t>(scan_data.at(15)), 0, 2);
	lineedit8->setText(info8);

	QLineEdit* lineedit9 = ui->lineEdit_9;
	QString info9 = QString("  %1 %2").arg(static_cast<uint8_t>(scan_data.at(16)), 0, 2).arg(static_cast<uint8_t>(scan_data.at(17)), 0, 2);
	lineedit9->setText(info9);

	QLineEdit* lineedit10 = ui->lineEdit_10;
	QString info10 = QString("  %1 %2").arg(static_cast<uint8_t>(scan_data.at(18)), 0, 2).arg(static_cast<uint8_t>(scan_data.at(19)), 0, 2);
	lineedit10->setText(info10);
}
