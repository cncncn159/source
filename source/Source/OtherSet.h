#pragma once

#include <QDialog>
#include <QLabel>
#pragma execution_character_set("utf-8")  

namespace Ui {
	class OtherSet;
}
class OtherSet : public QDialog
{
	Q_OBJECT

public:
	OtherSet(QWidget *parent = Q_NULLPTR);
	~OtherSet();

private:
	Ui::OtherSet* ui;
};
