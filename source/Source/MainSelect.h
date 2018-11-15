#pragma once
#include <QCheckbox>
#include <QDialog>
#pragma execution_character_set("utf-8")  
namespace Ui {
	class MainSelect;
}

class MainSelect : public QDialog
{
	Q_OBJECT

public:
	MainSelect(QWidget *parent = Q_NULLPTR);
	~MainSelect();

private:
	Ui::MainSelect* ui;
	QCheckBox* checkbox1;
	QCheckBox* checkbox2;
	QCheckBox* checkbox3;
	QCheckBox* checkbox4;
	QCheckBox* checkbox5;
	QCheckBox* checkbox6;

};
