#pragma once

#include "messager.h"
#include "tag_parser.h"

namespace HttpLib
{
	class HttpResponse;
}

namespace CrawlerImpl
{

class CrawlerSettings;
class CrawlerModel;

class CrawlerController 
	: public Messager
{
public:
	void setModel(CrawlerModel* model);
	void setSettings(CrawlerSettings* settings);

	CrawlerModel* model() const;
	CrawlerSettings* settings() const;

	void startCrawling(const std::atomic_bool& stopCrawling);

private:
	void handleRedirection(const HttpLib::HttpResponse* response);

private:
	CrawlerModel* m_model;
	CrawlerSettings* m_settings;
};

}
