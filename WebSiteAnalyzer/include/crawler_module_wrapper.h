#pragma once

#include "stdlibs.h"
#include "crawler_controller.h"
#include "crawler_model.h"

namespace WebSiteAnalyzer
{

class CrawlerModuleWrapper
{
public:


private:
	CrawlerImpl::CrawlerController m_controller;
	CrawlerImpl::CrawlerModel m_model;
};

}
