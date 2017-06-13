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

	bool isCrawlerInPending() const;
	void setAboutToStopFlag(bool value);

	bool isStopped() const;

	Q_SLOT void slot_startCrawler(CrawlerSettings* settings, bool aboutToContinue);
	Q_SLOT void slot_stopCrawler();

	Q_SIGNAL void signal_addUrl(const std::string& url, 
		const std::string& title, 
		const std::string& description, 
		const std::string& charset, 
		int responseCode
	);

	Q_SIGNAL void signal_addExternalUrl(const std::string& url);
	Q_SIGNAL void signal_queueSize(std::size_t size, int queueType);

	Q_SIGNAL void signal_add404Url(const std::string& url);
	Q_SIGNAL void signal_addEmptyDescriptionUrl(const std::string& url);
	Q_SIGNAL void signal_addEmptyTitleUrl(const std::string& url);

	Q_SIGNAL void signal_addDuplicatedTitleUrl(const std::string& url, const std::string& title, const std::string& charset);
	Q_SIGNAL void signal_addDuplicatedDescriptionUrl(const std::string& url, const std::string& description, const std::string& charset);

	Q_SIGNAL void signal_DNSError();
	Q_SIGNAL void signal_progressStopped();
	
protected:
	virtual void receiveMessage(const IMessage& message);

private:
	std::unique_ptr<CrawlerModel> m_model;
	std::unique_ptr<CrawlerController> m_controller;

	std::atomic_bool m_needToStopCrawling;
	std::atomic_bool m_crawlerInPending;
};

}
