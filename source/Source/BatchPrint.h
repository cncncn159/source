#pragma once

#include <QDialog>
#include <QCheckbox>
#include <QLabel>
#pragma execution_character_set("utf-8")  

namespace Ui {
	class BatchPrint;
}
class BatchPrint : public QDialog
{
	Q_OBJECT

public:
	BatchPrint(QWidget *parent = Q_NULLPTR);
	~BatchPrint();

private:
	Ui::BatchPrint* ui;
	QCheckBox* chebox1;
	QCheckBox* chebox2;
	QCheckBox* chebox3;
};
