#pragma once

#include <QDialog>
#include <QLabel>
#pragma execution_character_set("utf-8")  

namespace Ui {
	class ParameterSet;
}
class ParameterSet : public QDialog
{
	Q_OBJECT

public:
	ParameterSet(QWidget *parent = Q_NULLPTR);
	~ParameterSet();

private:
	Ui::ParameterSet* ui;
};
