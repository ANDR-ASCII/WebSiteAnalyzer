#include "messager.h"
#include "imessage_receiver.h"

namespace CrawlerImpl
{

void Messager::sendMessage(const IMessage& message) const noexcept
{
	for (auto receiver : m_receivers)
	{
		receiver->receiveMessage(message);
	}
}

void Messager::addReceiver(IMessageReceiver* receiver)
{
	if (std::find(std::begin(m_receivers), std::end(m_receivers), receiver) == std::end(m_receivers))
	{
		m_receivers.push_back(receiver);
	}
}

void Messager::deleteReceiver(IMessageReceiver* receiver)
{
	std::vector<IMessageReceiver*>::iterator iter = 
		std::find(std::begin(m_receivers), std::end(m_receivers), receiver);

	if (iter != std::end(m_receivers))
	{
		m_receivers.erase(iter);
	}
}

}