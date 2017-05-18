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
				if (!isItemExistsIn(currentUrl, ExternalCrawledUrlQueue) && !isItemExistsIn(currentUrl, ExternalUrlQueue))
				{
					storeUrl(currentUrl, ExternalUrlQueue);

					sendMessage(UrlMessage{ currentUrl.host(), 200, CrawlerModel::ExternalUrlQueue });
				}

				continue;
			}

			if (currentUrl.isAbsoluteAddress() && currentUrl.compareHost(hostUrl))
			{
				if (!isItemExistsIn(currentUrl, InternalCrawledUrlQueue))
				{
					storeUrl(currentUrl, InternalUrlQueue);
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
				storeUrl(url, InternalUrlQueue);
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

	if (!isItemExistsIn(url, queueType))
	{
		queue(queueType)->push_back(isInternalQueueType ? url.relativePath() : url.host());
	}
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

bool CrawlerModel::duplicateTitle(const Url& url, const std::string& title) const
{
	auto iter = m_duplicatedTitles.find(title);

	if (iter != std::end(m_duplicatedTitles))
	{
		return iter->second.size() > 1;
	}

	return false;
}

bool CrawlerModel::duplicateDescription(const Url& url, const std::string& description) const
{
	const Queue& queue = m_duplicatedDescriptions.find(description)->second;
	return std::find(std::begin(queue), std::end(queue), url.relativePath()) != std::end(queue);
}

void CrawlerModel::addTitle(const Url& url, const std::string& title)
{
	auto iter = m_duplicatedTitles.find(title);

	if (iter != std::end(m_duplicatedTitles))
	{
		iter->second.push_back(url.relativePath());
	}
	else
	{
		Queue queue;
		queue.push_back(url.relativePath());

		m_duplicatedTitles.insert(std::make_pair(title, std::move(queue)));
	}
}

void CrawlerModel::addDescription(const Url& url, const std::string& description)
{
	auto iter = m_duplicatedDescriptions.find(description);

	if (iter != std::end(m_duplicatedDescriptions))
	{
		iter->second.push_back(url.relativePath());
	}
	else
	{
		Queue queue;
		queue.push_back(url.relativePath());

		m_duplicatedDescriptions.insert(std::make_pair(description, std::move(queue)));
	}
}

}