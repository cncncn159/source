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


	checkbox1->setText("打印");
	checkbox2->setText("皮烫印");
	checkbox3->setText("打印结束走纸");
	checkbox4->setText("报打印有日期");
	checkbox5->setText("全显示方式");
	checkbox6->setText("英文格式打印");
}

MainSelect::~MainSelect()
{
}
