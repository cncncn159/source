#ifndef CBRIDGE_H
#define CBRIDGE_H

#include <QObject>

class CMsgBusiness;
class CPort;
class CBridge : public QObject
{
    Q_OBJECT
public:
    explicit CBridge(QObject *parent = nullptr);
    ~CBridge();
    CMsgBusiness* business()const;
    CPort* port()const;

signals:
    //这里可以转发信号
public slots:
private:
    CMsgBusiness* pBusiness;
    CPort* pPort;
};

#endif // CBRIDGE_H
