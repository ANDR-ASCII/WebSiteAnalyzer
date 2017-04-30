#include "crawler_base.h"
#include "general_helper_functions.h"

namespace CrawlerImpl
{

const std::string CrawlerBase::s_storage_of_phrases = "phrases.txt";
const std::string CrawlerBase::s_reportUrl = "url_crash_report.log";
const std::string CrawlerBase::s_storage_urls = "table_of_addresses.ta";

// second argument expect relative address with specified name file or without it
void CrawlerBase::addToQueue(const TagParser& parser, const Url& relativePath, UrlType urls)
{
	Url analyzeUrl;
	std::string resultAddr;

	for (const auto& tag : parser)
	{
		try
		{
			analyzeUrl.parse(tag.attribute("href"));

			// add only web executable files
			if (!analyzeUrl.file().empty() &&
				analyzeUrl.fileType() != Url::FileType::ExecutableWebFile)
			{
				continue;
			}

			if (analyzeUrl.isAbsoluteAddress())
			{
				if (!analyzeUrl.compareHost(m_host) &&
					urls == UrlType::Default || urls == UrlType::External)
				{
					// found external link
					if (!existsInQueues(analyzeUrl, External))
					{
						// temporary compare
						// while HttpLib don't support https
						if (analyzeUrl.protocol() != "https://")
						{
							// add subdomain if exists
							m_externalLinks.push_back(analyzeUrl.host());
						}
					}
					else
					{
						continue;
					}

					continue;
				}
			}

			if (analyzeUrl.isAnchor())
			{
				continue;
			}

			/**
			** WARNING
			** There is a possibility that there will be 2 identical addresses: / and /index.php often equivalent
			**/

			/** Second part. Handling relative path. **/

			// can throw std::runtime_error
			resultAddr = convertRelativeAddress(analyzeUrl, relativePath);

			if (!existsInQueues(resultAddr) &&
				urls == UrlType::Internal || urls == UrlType::Default)
			{
				m_internalLinks.push_back(resultAddr);
			}

		}
		catch (UrlParseErrorException const& parsingUrlError)
		{
			// If failed parse link and object of Link throws exception
			// We create report consisting from address
			// And continues data handling

			std::ofstream crash(s_reportUrl, std::ios_base::app);

			crash << parsingUrlError.what() << std::endl;
		}
	}
}

// true if found specified link in specified queue, otherwise false
bool CrawlerBase::existsInQueues(const Url& url, int queueType)
{
	if (queueType == Internal)
	{
		for (const auto& item : m_indexedInternal)
		{
			if (item == url.relativePath())
			{
				return true;
			}
		}

		for (const auto& item : m_internalLinks)
		{
			if (item == url.relativePath())
			{
				return true;
			}
		}

		return false;
	}

	if (queueType == External)
	{
		for (const auto& item : m_indexedExternal)
		{
			// compare with full address
			if (url.compareHost(item))
			{
				return true;
			}
		}

		for (const auto &item : m_externalLinks)
		{
			// compare with full address
			if (url.compareHost(item))
			{
				return true;
			}
		}
	}

	return false;
}

// add link into specified queue with check on uniqueness
// if addition into internal queue then adds only relative path.
// if addition into external queue then adds url.getMainAddress()
void CrawlerBase::addLinkToQueue(const Url& url, int whereToSearch)
{
	if (whereToSearch == Internal)
	{
		if (!existsInQueues(url))
		{
			m_internalLinks.push_back(url.relativePath());
		}
		return;
	}

	if (whereToSearch == IndexedIn)
	{
		if (!existsInQueues(url))
		{
			m_indexedInternal.push_back(url.relativePath());
		}
		return;
	}

	if (whereToSearch == External)
	{
		if (!existsInQueues(url, whereToSearch))
		{
			m_externalLinks.push_back(url.host());
		}
		return;
	}

	if (whereToSearch == IndexedEx)
	{
		if (!existsInQueues(url, whereToSearch))
		{
			m_indexedExternal.push_back(url.host());
		}
		return;
	}
}

void CrawlerBase::clearQueue(int queueType)
{
	if ((queueType & Internal) == Internal)
	{
		m_internalLinks.clear();
	}
	if ((queueType & IndexedIn) == IndexedIn)
	{
		m_indexedInternal.clear();
	}
	if ((queueType & External) == External)
	{
		m_externalLinks.clear();
	}
	if ((queueType & IndexedEx) == IndexedEx)
	{
		m_indexedExternal.clear();
	}
}

// converts a relative path in accordance with the place where the received address
std::string CrawlerBase::convertRelativeAddress(const Url& relAddr, const Url& where)
{
	// if path specified beginning slash
	// it means path specified relative by root of website
	// returns it
	if (relAddr.relativePath()[0] == '/')
	{
		return relAddr.relativePath();
	}

	if (relAddr.relativePath().empty() && where.relativeDir().empty())
	{
		// root
		return "/";
	}

	if (relAddr.relativePath().empty())
	{
		return where.relativePath();
	}

	if (where.relativeDir().empty())
	{
		return relAddr.relativePath();
	}

	/** Handling **/
	std::vector<std::string> dirFirst;
	std::vector<std::string> dirSecond;

	if (!relAddr.relativeDir().empty())
	{
		dirFirst = dividePath(relAddr.relativeDir());
	}

	dirSecond = dividePath(where.relativeDir());

	std::size_t sizeFirst = dirFirst.size();
	std::size_t sizeSecond = dirSecond.size();

	std::size_t counter = 0;
	std::vector<std::size_t> indexDots; // consisting indexes of elements of dots in path ./folder/

	for (std::size_t i = 0; i < sizeFirst; ++i)
	{
		if (dirFirst[i] == "..")
		{
			++counter;
		}

		if (dirFirst[i] == ".")
		{
			indexDots.push_back(i);
		}
	}

	std::string result;

	// delete as many folders as met ".."
	for (std::size_t i = 0; i < sizeSecond && i < counter; ++i)
	{
		dirSecond.pop_back();
	}

	// resize
	sizeSecond = dirSecond.size();

	// build parts of path, where received link
	for (std::size_t i = 0; i < sizeSecond; ++i)
	{
		result += dirSecond[i] + '/';
	}

	// build link
	for (auto i = counter; i < sizeFirst; ++i)
	{
		if (!General::findValueInVector(indexDots, i))
		{
			result += dirFirst[i] + '/';
		}
	}

	// add name of file and vars
	result += relAddr.file() + relAddr.variablesString();

	return result[0] != '/' ? ('/' + result) : result;
}

// divide path by folders
// expects path like this: [/]aaa/[bbb/[page.php[?var1=val&var2=val]]]
// require last slash
std::vector<std::string> CrawlerBase::dividePath(const std::string& path)
{
	if (path.empty())
	{
		return std::vector<std::string>();
	}

	std::vector<std::string> dirs;
	std::string::size_type pointer = path.find('/');

	if (pointer != std::string::npos)
	{
		std::string tmpFolder;
		std::string::size_type pos = 0;

		do
		{
			tmpFolder.assign(path, pos, !pos ? pointer : pointer - pos);
			dirs.push_back(tmpFolder);
			pos = pointer + 1;

		} while ((pointer = path.find('/', pos)) != std::string::npos);
	}

	return dirs;
}

}