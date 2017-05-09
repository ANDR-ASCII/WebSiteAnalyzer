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

		case CrawlerImpl::IMessage::MessageType::WarningType:
		{
			const CrawlerImpl::WarningMessage& actualMessage =
				static_cast<const CrawlerImpl::WarningMessage&>(message);

			emit signal_warningMessage(actualMessage.warning());

			break;
		}
	}
}

}