#include "crawler_model.h"
#include "message_types.h"

namespace CrawlerImpl
{

void CrawlerModel::saveUniqueUrls(const TagParser& tagParser, const Url& hostUrl, const Url& containingUrl)
{
	Url currentUrl;
	WarningMessage httpsWarning{ "Current HTTP library does not support HTTPS protocol" };

	assert(tagParser.countTag("a") == tagParser.size());

	for (const Tag& tag : tagParser)
	{
		try
		{

			currentUrl.parse(tag.attribute("href"));

			if (!currentUrl.file().empty() && 
				currentUrl.fileType() != Url::FileType::ExecutableWebFile ||
				currentUrl.isAnchor())
			{
				continue;
			}

			if (currentUrl.isAbsoluteAddress() && !currentUrl.compareHost(hostUrl))
			{
				if (!isItemExistsIn(currentUrl, ExternalCrawledUrlQueue))
				{
					m_externalUrlQueue.push_back(currentUrl.host());
				}

				continue;
			}

			if (currentUrl.isAbsoluteAddress() && currentUrl.compareHost(hostUrl))
			{
				if (!isItemExistsIn(currentUrl, InternalCrawledUrlQueue))
				{
					queue(InternalUrlQueue)->push_back(currentUrl.relativePath());
				}

				continue;
			}

			//
			// WARNING:
			// There is a possibility that there will be 2 identical addresses: / and /index.php often equivalent
			//

			Url url = currentUrl.mergeRelativePaths(currentUrl, containingUrl);

			if (!isItemExistsIn(url, InternalCrawledUrlQueue))
			{
				queue(InternalUrlQueue)->push_back(url.relativePath());
			}
		}
		catch (UrlParseErrorException const& parsingUrlError)
		{
			sendMessage(WarningMessage{ parsingUrlError.what() });
		}
	}

	sendMessage(QueueSizeMessage{ InternalUrlQueue, size(InternalUrlQueue) });
}

void CrawlerModel::storeUrl(const Url& url, int queueType)
{
	const bool isInternalQueueType =
		queueType == InternalUrlQueue ||
		queueType == InternalCrawledUrlQueue;

	queue(queueType)->push_back(isInternalQueueType ? url.relativePath() : url.host());
}

bool CrawlerModel::isItemExistsIn(const Url& url, int queueType) const
{
	const bool isInternalQueueType =
		queueType == InternalUrlQueue ||
		queueType == InternalCrawledUrlQueue;

	Queue::const_iterator endIter = queue(queueType)->end();

	Queue::const_iterator findIter = 
		std::find(queue(queueType)->begin(), queue(queueType)->end(), 
			isInternalQueueType ? url.relativePath() : url.host());

	return findIter != endIter;
}

std::size_t CrawlerModel::size(int queueType) const noexcept
{
	return queue(queueType)->size();
}

CrawlerModel::Queue* CrawlerModel::queue(int queueType) noexcept
{
	const Queue* queuePointer = 
		const_cast<CrawlerModel const * const>(this)->queue(queueType);
	
	return const_cast<Queue*>(queuePointer);
}

const CrawlerModel::Queue* CrawlerModel::queue(int queueType) const noexcept
{
	assert(queueType == InternalUrlQueue ||
		queueType == InternalCrawledUrlQueue ||
		queueType == ExternalUrlQueue ||
		queueType == ExternalCrawledUrlQueue
	);

	if (queueType == InternalUrlQueue)
	{
		return &m_internalUrlQueue;
	}

	if (queueType == InternalCrawledUrlQueue)
	{
		return &m_internalCrawledUrlQueue;
	}

	if (queueType == ExternalUrlQueue)
	{
		return &m_externalUrlQueue;
	}

	return &m_externalUrlQueue;
}

}