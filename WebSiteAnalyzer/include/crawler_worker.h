#pragma once

#include "stdlibs.h"
#include "crawler_controller.h"
#include "crawler_model.h"
#include "crawler_settings.h"

namespace WebSiteAnalyzer
{

using namespace CrawlerImpl;

class CrawlerWorker 
	: public QObject
	, public IMessageReceiver
{
	Q_OBJECT

public:
	CrawlerWorker();

	Q_SLOT void slot_startCrawler(CrawlerSettings* settings);
	Q_SLOT void slot_stopCrawler();

	Q_SIGNAL void signal_addUrl(const std::string& url, int responseCode);
	Q_SIGNAL void signal_queueSize(std::size_t size, int queueType);
	Q_SIGNAL void signal_DNSError();
	
protected:
	virtual void receiveMessage(const IMessage& message);

private:
	std::unique_ptr<CrawlerModel> m_model;
	std::unique_ptr<CrawlerController> m_controller;

	std::atomic_bool m_needToStopCrawling;
};

}
