#include "BatchPrint.h"
#include "ui_BatchPrint.h"

BatchPrint::BatchPrint(QWidget *parent)
	: QDialog(parent),
	ui(new Ui::BatchPrint)

{
	ui->setupUi(this);
	chebox1 = ui->checkBox;
	chebox2 = ui->checkBox_2;
	chebox3 = ui->checkBox_3;
	QLabel* text1 = ui->label;
	text1->setText("批打印项");
	chebox1->setText("简单");
	chebox2->setText("简明");
	chebox3->setText("详细");
}

BatchPrint::~BatchPrint()
{
}
