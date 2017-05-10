#pragma once

#include "stdlibs.h"
#include "url.h"
#include "tag_parser.h"
#include "messager.h"
#include "imessage_receiver.h"

namespace CrawlerImpl
{

using namespace HtmlParser;

class CrawlerModel
	: public Messager
{
private:
	using Queue = std::deque<std::string>;

public:
	enum QueueType
	{
		InternalUrlQueue,
		InternalCrawledUrlQueue,
		ExternalUrlQueue,
		ExternalCrawledUrlQueue
	};

	//!
	//! First argument contains tags 'a'
	//! Second argument determine host where urls from first argument parsed
	//! Third argument determine url where urls from first argument parsed
	//!
	void saveUniqueUrls(const TagParser& tagParser, const Url& hostUrl, const Url& containingUrl);
	void storeUrl(const Url& url, int queueType);
	bool isItemExistsIn(const Url& url, int queueType) const;

	std::size_t size(int queueType) const noexcept;

	Queue* queue(int queueType) noexcept;
	const Queue* queue(int queueType) const noexcept;

private:
	Queue m_internalUrlQueue;
	Queue m_internalCrawledUrlQueue;

	Queue m_externalUrlQueue;
	Queue m_externalCrawledUrlQueue;
};

}
