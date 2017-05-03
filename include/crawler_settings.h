#pragma once

#include "stdlibs.h"
#include "url.h"

namespace CrawlerImpl
{

using namespace HtmlParser;

class CrawlerSettings
{
public:
	void setHost(const Url& url);
	void setRequestPause(std::chrono::milliseconds pause) noexcept;
	void setSignature(const std::string& signature);
	void setMaxCrawlLinks(std::size_t value) noexcept;
	void setInfiniteCrawling(bool value) noexcept;

	const Url& startUrlAddress() const noexcept;
	std::chrono::milliseconds requestPause() const noexcept;
	const std::string& signature() const noexcept;
	std::size_t maxCrawlLinks() const noexcept;
	bool infiniteCrawling() const noexcept;

	bool readyForStart() const noexcept;

private:
	Url m_hostStartAddress;
	
	std::chrono::milliseconds m_requestPause;

	std::string m_signature;
	
	std::size_t m_maxCrawlLinks;

	bool m_infiniteCrawling;
};

}