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
	text1->setText("����ӡ��");
	chebox1->setText("��");
	chebox2->setText("����");
	chebox3->setText("��ϸ");
}

BatchPrint::~BatchPrint()
{
}
