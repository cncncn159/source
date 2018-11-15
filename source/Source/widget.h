#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include "scantest.h"
#include "MainSelect.h"
#include "BatchPrint.h"
#include "ParameterSet.h"
#include "OtherSet.h"
#include "cbridge.h"
#include "cmsgbusiness.h"
#include "uartthread.h"

#define MEASURING_TIMEOUT	15//�����г�ʱʱ�� ��λs

namespace Ui {
class Widget;
}

typedef enum
{
	E_UINT_M2,
	E_UINT_YC,
	E_UINT_SC
}eUintType;

typedef enum
{
	E_CARRY_0,
	E_CARRY_00,
	E_CARRY_000
}eCarryType;

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
	uint8_t hard_version;
	/*ɨ�������״̬*/
	uint8_t scanbroad_net_sta;	//0 δ���� 1 ����
	/*ɨ������*/
	QByteArray scan_arry;
	uint8_t mesuring_flag;
	QTimer* measuring_timer;
	uint16_t accu_area;	//ÿ��Ƥ���������
	float disp_accu;	//Ҫ��ʾ�������
	/*����*/
	uint16_t H_CrimpySelect;	//������
	uint16_t H_SynchronalSelect;//ͬ����
	eUintType uint_flag;			//��ǰ��λ
	eCarryType carry_flag;			//��ǰ����
	float oldFloat;
	uint32_t SheetArea;
	uint16_t CarryUint(float oldFloat, uint8_t carrytype);
	float AreaCoefficient(eUintType flag);
	int Widget::float_to_int(float f);
private slots:
    void recSerialData(void);
    void on_pushButton_clicked();
	void on_pushButton_MainSelect();
	void on_pushButton_BatchPrint();
	void on_pushButton_ParaSet();
	void on_pushButton_OtherSet();
	void mearing_timeout(void);
	void timerOutSed(void);
public slots:
	void onComing(QByteArray content);

};

#endif // WIDGET_H
