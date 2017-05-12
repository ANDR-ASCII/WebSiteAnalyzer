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

			emit signal_addUrl(actualMessage.url());

			break;
		}

		case IMessage::MessageType::CurrentQueueSize:
		{
			const QueueSizeMessage& actualMessage =
				static_cast<const QueueSizeMessage&>(message);

			emit signal_queueSize(actualMessage.size(), actualMessage.queueType());

			break;
		}
	}
}

void CrawlerWorker::slot_startCrawler(CrawlerSettings* settings)
{
	m_controller->setModel(m_model.get());
	m_controller->setSettings(settings);
	m_controller->addReceiver(this);

	m_controller->startCrawling(m_needToStopCrawling);

	m_controller->deleteReceiver(this);

	m_needToStopCrawling.store(false);
}

void CrawlerWorker::slot_stopCrawler()
{
	m_needToStopCrawling.store(true);
}

}