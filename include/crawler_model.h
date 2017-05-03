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
	using Queue = std::unordered_set<Url, UrlHash<Url>>;

public:
	enum QueueType
	{
		InternalUrlQueue,
		InternalCrawledUrlQueue,
		ExternalUrlQueue,
		ExternalCrawledUrlQueue
	};

	class SmartModelElement
		: public IMessageReceiver
	{
	private:
		SmartModelElement(const Url* url, CrawlerModel* model, int queueType);

		friend class CrawlerModel;

	protected:
		void receiveMessage(const IMessage* message) override;

	public:
		~SmartModelElement();

		const Url* value() const noexcept;

	private:
		const Url* m_url;
		CrawlerModel* m_model;
		int m_queueType;
		bool m_pointerInvalidated;
	};

	using SmartModelElementPtr = std::shared_ptr<SmartModelElement>;

	//!
	//! First argument contains tags 'a'
	//! Second argument determine host where urls from first argument parsed
	//! Third argument determine url where urls from first argument parsed
	//!
	void saveUniqueUrls(const TagParser& tagParser, const Url& hostUrl, const Url& containingUrl);

	void storeUrl(const Url& url, int queueType);

	//!
	//! Value from this function may invalidate
	//! After any change model operation
	//!
	SmartModelElementPtr anyUrl(int queueType);

	std::size_t size(int queueType) const noexcept;

	// returns pointer to the element in the new queue
	SmartModelElementPtr moveUrl(const Url& urlKey, int fromQueueType, int toQueueType);

private:
	Queue* queue(int queueType) noexcept;
	const Queue* queue(int queueType) const noexcept;

private:
	Queue m_internalUrlQueue;
	Queue m_internalCrawledUrlQueue;

	Queue m_externalUrlQueue;
	Queue m_externalCrawledUrlQueue;
};

}
