#pragma once

#include "messager.h"

namespace CrawlerImpl
{

struct IMessageReceiver
{
	virtual void receiveMessage(const IMessage* message) = 0;
};

}
