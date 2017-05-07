#pragma once

#include "url.h"
#include "any.h"
#include "message_types.h"

namespace CrawlerImpl
{

struct IMessageReceiver;

class Messager
{
public:
	void sendMessage(const IMessage& message) const noexcept;
	void addReceiver(IMessageReceiver* receiver);
	void deleteReceiver(IMessageReceiver* receiver);

private:
	std::vector<IMessageReceiver*> m_receivers;
};

}