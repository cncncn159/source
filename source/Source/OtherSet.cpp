#include "OtherSet.h"
#include "ui_OtherSet.h"

OtherSet::OtherSet(QWidget *parent)
	: QDialog(parent),
	ui(new Ui::OtherSet)
{
	ui->setupUi(this);
	QLabel* label = ui->label;
	label->setText("Æ¤¸ï¼Û¸ñ");
}

OtherSet::~OtherSet()
{
}
