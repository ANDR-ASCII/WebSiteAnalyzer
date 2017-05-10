#pragma once

#include "stdlibs.h"
#include "crawler_controller.h"
#include "crawler_model.h"
#include "crawler_settings.h"

namespace WebSiteAnalyzer
{

using namespace CrawlerImpl;

class CrawlerWorker : public QObject
{
	Q_OBJECT

public:
	CrawlerWorker();

	Q_SLOT void startCrawler(const CrawlerSettings* settings);

private:
	std::unique_ptr<CrawlerModel> m_model;
	std::unique_ptr<CrawlerController> m_controller;
};

}
