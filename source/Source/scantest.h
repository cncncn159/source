#ifndef SCANTEST_H
#define SCANTEST_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QTimer>
#include <QBytearray>
#include "sys.h"

#define SCAN_REFRESH_INTERVAL	1000		//发送扫描检测间隔 单位ms

namespace Ui {
class ScanTest;
}

class ScanTest : public QDialog
{
    Q_OBJECT

public:
    explicit ScanTest(QWidget *parent = 0);
    ~ScanTest();
signals:
	void sedScanFrame(void);
private:
    Ui::ScanTest *ui;
	QTimer* timer_refresh;
	QByteArray scan_data;
	QPushButton* ButtonDis;
	QPushButton* ButtonDis2;
private slots:
    void showValue(QByteArray value);
	void timerOutSed(void);
};

#endif // SCANTEST_H
