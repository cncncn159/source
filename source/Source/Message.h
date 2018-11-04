#pragma once
#include <QBytearray>

class Message
{
public:
	Message();
	~Message();
    QByteArray content;
};

