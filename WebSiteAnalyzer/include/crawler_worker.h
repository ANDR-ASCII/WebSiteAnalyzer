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

	virtual void receiveMessage(const IMessage& message);

	Q_SLOT void slot_startCrawler(CrawlerSettings* settings);
	Q_SIGNAL void signal_addUrl(const std::string& url);

private:
	std::unique_ptr<CrawlerModel> m_model;
	std::unique_ptr<CrawlerController> m_controller;

	std::atomic_bool m_needToStopCrawling;
};

}
