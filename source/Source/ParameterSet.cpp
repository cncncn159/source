#include "ParameterSet.h"
#include "ui_ParameterSet.h"

ParameterSet::ParameterSet(QWidget *parent)
	: QDialog(parent),
	ui(new Ui::ParameterSet)
{
	ui->setupUi(this);
	QLabel* text1 = ui->label;
	QLabel* text2 = ui->label_2;

	text1->setText("面积下限");
	text2->setText("箱包设定");
}

ParameterSet::~ParameterSet()
{
}
