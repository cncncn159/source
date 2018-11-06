#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include "scantest.h"
#include "cbridge.h"
#include "cmsgbusiness.h"
#include "uartthread.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent,CBridge* bridge);
    ~Widget();

signals:
//    void readyRead();
//    void dlgReturn(QByteArray);
	void UiSometingDoing(QByteArray);
private:
    Ui::Widget *ui;
    QStringList baudList;
    void initPort(void);
    void initText(void);
    void closeSerialPort(void);
    QSerialPort *serial;
    void handleError(QSerialPort::SerialPortError error);
    UartThread* uart;
	CBridge* pb;
private slots:
    void recSerialData(void);
	void onComing(QByteArray content);
    void on_pushButton_clicked();
};

#endif // WIDGET_H
