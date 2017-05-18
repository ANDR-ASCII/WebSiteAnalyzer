#include "crawler_worker.h"

namespace WebSiteAnalyzer
{

CrawlerWorker::CrawlerWorker()
	: m_model(new CrawlerModel)
	, m_controller(new CrawlerController)
	, m_needToStopCrawling(false)
{
}

void CrawlerWorker::receiveMessage(const IMessage& message)
{
	switch (message.type())
	{
		case IMessage::MessageType::Url:
		{
			const UrlMessage& actualMessage =
				static_cast<const UrlMessage&>(message);

			if (actualMessage.queueType() == CrawlerModel::ExternalUrlQueue)
			{
				emit signal_addExternalUrl(actualMessage.url());

				return;
			}

			if (actualMessage.responseCode() == 404)
			{
				emit signal_add404Url(actualMessage.url());
			}
			
			emit signal_addUrl(actualMessage.url(), actualMessage.responseCode());

			break;
		}

		case IMessage::MessageType::CurrentQueueSize:
		{
			const QueueSizeMessage& actualMessage =
				static_cast<const QueueSizeMessage&>(message);

			emit signal_queueSize(actualMessage.size(), actualMessage.queueType());

			break;
		}

		case IMessage::MessageType::DNSError:
		{
			emit signal_DNSError();

			break;
		}

		//
		// errors handling
		//

		case IMessage::MessageType::DuplicatedTitle:
		{
			const DuplicatedTitleMessage& actualMessage =
				static_cast<const DuplicatedTitleMessage&>(message);

			emit signal_addDuplicatedTitleUrl(actualMessage.url(), actualMessage.title());

			break;
		}

		case IMessage::MessageType::DuplicatedDescription:
		{
			const DuplicatedDescriptionMessage& actualMessage =
				static_cast<const DuplicatedDescriptionMessage&>(message);

			emit signal_addDuplicatedTitleUrl(actualMessage.url(), actualMessage.description());

			break;
		}

		case IMessage::MessageType::EmptyTitle:
		{
			const EmptyTitleMessage& actualMessage =
				static_cast<const EmptyTitleMessage&>(message);

			emit signal_addEmptyTitleUrl(actualMessage.url());

			break;
		}

		case IMessage::MessageType::EmptyDescription:
		{
			const EmptyDescriptionMessage& actualMessage =
				static_cast<const EmptyDescriptionMessage&>(message);

			emit signal_addEmptyDescriptionUrl(actualMessage.url());

			break;
		}
	}
}

void CrawlerWorker::slot_startCrawler(CrawlerSettings* settings)
{
	//
	// when closes program this thread will attempt to access
	// to deleted object. Need to copy
	//
	CrawlerSettings selfSettings = *settings;

	m_controller->setModel(m_model.get());
	m_controller->setSettings(&selfSettings);
	m_controller->addReceiver(this);
	m_model->addReceiver(this);

	m_controller->startCrawling(m_needToStopCrawling);

	m_controller->deleteReceiver(this);
	m_model->deleteReceiver(this);

	m_needToStopCrawling.store(false);
}

void CrawlerWorker::slot_stopCrawler()
{
	m_needToStopCrawling.store(true);
}

}