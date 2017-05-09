#include "message_mapper.h"

namespace WebSiteAnalyzer
{

void MessageMapper::receiveMessage(const CrawlerImpl::IMessage& message)
{
	switch (message.type())
	{
		case CrawlerImpl::IMessage::MessageType::CurrentRequestedUrl:
		{
			const CrawlerImpl::CurrentRequestedUrlMessage& actualMessage =
				static_cast<const CrawlerImpl::CurrentRequestedUrlMessage&>(message);

			emit signal_modelUpdated();

			break;
		}
	}
}

}