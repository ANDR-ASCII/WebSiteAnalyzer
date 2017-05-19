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
		InternalUrlQueue = 1 << 0,
		InternalCrawledUrlQueue = 1 << 1,
		ExternalUrlQueue = 1 << 2,
		ExternalCrawledUrlQueue = 1 << 3
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

	bool isDuplicatedTitle(const Url& url, const std::string& title) const;
	bool isDuplicatedDescription(const Url& url, const std::string& description) const;

	size_t duplicatesTitle(const Url& url, const std::string& title) const;
	size_t duplicatesDescription(const Url& url, const std::string& description) const;

	const std::string& firstDuplicatedTitle(const std::string& title) const;
	const std::string& firstDuplicatedDescription(const std::string& description) const;

	void addTitle(const Url& url, const std::string& title);
	void addDescription(const Url& url, const std::string& description);

private:
	Queue m_internalUrlQueue;
	Queue m_internalCrawledUrlQueue;

	Queue m_externalUrlQueue;
	Queue m_externalCrawledUrlQueue;

	std::unordered_map<std::string, Queue> m_duplicatedTitles;
	std::unordered_map<std::string, Queue> m_duplicatedDescriptions;
};

}
