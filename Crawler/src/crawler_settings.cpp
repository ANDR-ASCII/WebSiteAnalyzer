#include "crawler_settings.h"

namespace CrawlerImpl
{

void CrawlerSettings::setHost(const Url& url)
{
	m_hostStartAddress = url;
}

void CrawlerSettings::setRequestPause(std::chrono::milliseconds pause) noexcept
{
	m_requestPause = pause;
}

void CrawlerSettings::setSignature(const std::string& signature)
{
	m_signature = signature;
}

void CrawlerSettings::setMaxCrawlUrls(std::size_t value) noexcept
{
	m_maxCrawlLinks = value;
}

void CrawlerSettings::setInfiniteCrawling(bool value) noexcept
{
	m_infiniteCrawling = value;
}

const HtmlParser::Url& CrawlerSettings::startUrlAddress() const noexcept
{
	return m_hostStartAddress;
}

std::chrono::milliseconds CrawlerSettings::requestPause() const noexcept
{
	return m_requestPause;
}

const std::string& CrawlerSettings::signature() const noexcept
{
	return m_signature;
}

std::size_t CrawlerSettings::maxCrawlLinks() const noexcept
{
	return m_maxCrawlLinks;
}

bool CrawlerSettings::infiniteCrawling() const noexcept
{
	return m_infiniteCrawling;
}

bool CrawlerSettings::readyForStart() const noexcept
{
	return startUrlAddress().isAbsoluteAddress();
}

}