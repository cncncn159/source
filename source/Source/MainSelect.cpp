#include "MainSelect.h"
#include "ui_MainSelect.h"

MainSelect::MainSelect(QWidget *parent)
	: QDialog(parent),
	ui(new Ui::MainSelect)
{
	ui->setupUi(this);
	checkbox1 = ui->checkBox;
	checkbox2 = ui->checkBox_2;
	checkbox3 = ui->checkBox_3;
	checkbox4 = ui->checkBox_4;
	checkbox5 = ui->checkBox_5;
	checkbox6 = ui->checkBox_6;


	checkbox1->setText("��ӡ");
	checkbox2->setText("Ƥ��ӡ");
	checkbox3->setText("��ӡ������ֽ");
	checkbox4->setText("����ӡ������");
	checkbox5->setText("ȫ��ʾ��ʽ");
	checkbox6->setText("Ӣ�ĸ�ʽ��ӡ");
}

MainSelect::~MainSelect()
{
}
