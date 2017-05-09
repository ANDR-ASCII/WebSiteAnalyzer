#pragma once

#include "stdlibs.h"
#include "main_frame.h"

namespace WebSiteAnalyzer
{

class Application : public QObject
{
	Q_OBJECT

public:
	Application(int argc, char** argv);

	void crawlerThread();
	int exec();

	Q_SLOT void slot_stopCrawlerCommand();

private:
	Q_SLOT void slot_startCrawlingCommand(CrawlerImpl::CrawlerSettings* settings);

private:
	QApplication m_app;

	std::unique_ptr<CrawlerImpl::CrawlerModel> m_model;
	std::unique_ptr<CrawlerImpl::CrawlerController> m_controller;

	std::atomic_bool m_needToStopCrawler;
	std::atomic_bool m_crawlerActuallyStopped;
	std::condition_variable m_crawlerStopped;
	std::mutex m_mutex;

	std::unique_ptr<MainFrame> m_mainFrame;
};

}
