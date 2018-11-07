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
//	void UiSometingDoing(QByteArray);
	void scanValsed(QByteArray);
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
	QPushButton* scanButton;
	
	/*扫描板连接状态*/
	uint8_t scanbroad_net_sta;	//0 未连接 1 连接
	/*扫描数据*/
	QByteArray scan_arry;
private slots:
    void recSerialData(void);
    void on_pushButton_clicked();
public slots:
	void onComing(QByteArray content);

};

#endif // WIDGET_H
