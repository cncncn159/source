#include "cbridge.h"
#include "cport.h"
#include "cmsgbusiness.h"

CBridge::CBridge(QObject *parent) : QObject(parent)
{
    pBusiness = new CMsgBusiness(this);
    pPort = new CPort();
    //通讯层与业务层的通讯
    connect(pPort,SIGNAL(signalUpwardMsg(QByteArray)),pBusiness,SLOT(onCommingMsg(QByteArray)));
	connect(pBusiness, SIGNAL(signalToSend(QByteArray)), pPort, SLOT(onDownwardMsg(const QByteArray)));
}

CBridge::~CBridge()
{

}

CMsgBusiness *CBridge::business() const
{
    return pBusiness;
}

CPort* CBridge::port() const
{
    return pPort;
}
