#include "crawler_worker.h"

namespace WebSiteAnalyzer
{

CrawlerWorker::CrawlerWorker()
	: m_model(new CrawlerModel)
	, m_controller(new CrawlerController)
{
}

void CrawlerWorker::startCrawler(const CrawlerSettings* settings)
{

}

}