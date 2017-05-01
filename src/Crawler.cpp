#include "stdlibs.h"
#include "general_helper_functions.h"
#include "crawler.h"
#include "tag_parser.h"
#include "attribute.h"

// correct time
#define STD_PAUSE_BETWEEN_REQUESTS 3000

namespace CrawlerImpl
{

Crawler::Crawler()
	: m_timePause(STD_PAUSE_BETWEEN_REQUESTS)
	, m_sign("PasBot v1.0 Alpha ")
	, m_maxDepth(0)
	, m_readyForStart(false)
	, m_infiniteCrawl(false)
{
	m_signature = m_sign;
	m_request.setUserAgent(m_signature);
}

Crawler::Crawler(const std::string &startAddr)
	: m_timePause(STD_PAUSE_BETWEEN_REQUESTS)
	, m_sign("PasBot v1.0 Alpha ")
	, m_maxDepth(0)
	, m_infiniteCrawl(false)
{
	m_signature = m_sign;
	setStartAddress(startAddr);
	m_request.setUserAgent(m_signature);
}

Crawler::Crawler(const std::string &startAddr, std::size_t timePause)
	: m_timePause(timePause)
	, m_sign("PasBot v1.0 Alpha ")
	, m_maxDepth(0)
	, m_infiniteCrawl(false)
{
	m_signature = m_sign;
	setStartAddress(startAddr);
	m_request.setUserAgent(m_signature);
}

void Crawler::crawlStart()
{
	TagParser parser;

	// send on output device
	m_viewerDevice->viewInfo(m_host.host(),
		m_externalLinks.size(),
		m_internalLinks.size(),
		m_indexedInternal.size());

	// set configurations of request
	m_request.setHost(m_host.host());
	m_request.setHttpVersion(HttpRequest::Version::V_1_1);
	m_request.setConnectionType(HttpRequest::ConnectionType::Close);

	// stores the indexed link
	m_indexedInternal.push_back("/");

	try
	{
		// for correct output
		// if not set, print 200 OK, otherwise not
		bool mpOccur = false;
		std::this_thread::sleep_for(std::chrono::milliseconds(m_timePause));

		m_request.build();
		// get address of object response_ of controller
		m_response = m_controller.openUrl(m_request);

		if (m_response->isValid())
		{
			// HTTP error - 301 Moved Permanently
			while (m_response->is_301_MovedPermanently() || m_response->is_302_MovedTemporarily())
			{
				m_viewerDevice->viewResponseStatus(m_response->getCodeOfResponse());
				storeLinkToQueue(m_host, IndexedEx); // save current url

				// set flag
				mpOccur = !mpOccur;

				std::string tmpRefer, firstHost = m_host.host();
				m_response->getHeader("location", tmpRefer);
				m_host.parse(tmpRefer);

				// until HttpLib don't support https
				if (m_host.protocol() == "https://")
				{

					m_viewerDevice->viewWarning("****************************************************\n"
						"* To address this need support Https.              *\n"
						"* But HttpLib still does not support this protocol.*\n"
						"****************************************************\n");


					// jump into crawlResource with empty parameters
					// for the next crawl
					if (getMaxDepth() > 1)
					{
						crawlResource();
					}
				}

				if (m_host.isAbsoluteAddress())
				{
					m_request.setHost(m_host.host());
					m_request.setRelativePath(m_host.relativePath());
				}
				else if (m_host.isRelativeAddress())
				{
					// in order not to lose the host
					m_host = firstHost + convertRelativeAddress(m_host.relativePath(), "/");
					m_request.setRelativePath(m_host.relativePath());
				}

				m_request.build();

				// get address of object response_ of controller
				m_response = m_controller.openUrl(m_request);

				// send on output device
				m_viewerDevice->viewInfo(m_host.host(),
					m_externalLinks.size(),
					m_internalLinks.size(),
					m_indexedInternal.size());
			}

			m_viewerDevice->viewResponseStatus(m_response->getCodeOfResponse());

			// saves links
			parser.parseTags(m_response->getEntityBody(), "a");
			addToCrawlQueue(parser, m_indexedInternal.back());

			if (getMaxDepth() == 1)
			{
				return;
			}

			// calls indexer of website
			crawlResource();
		}
		else
		{
			m_viewerDevice->viewWarning("Response invalid\n");
		}
	}
	catch (HttpErrorException const& fh)
	{
		std::cout << "Web site unavailable.\n";
		std::ofstream fails("failConnections.log", std::ios_base::app);

		fails << "Address: " << m_request.host() + m_request.relativePath()
			<< std::endl << fh.what() << std::endl;

		if (getMaxDepth() > 1)
		{
			crawlResource();
		}
		else
		{
			return;
		}
	}
	catch(UrlParseErrorException const& err_url)
	{
	    // If failed parse link and object of Link throws exception
	    // We create report consisting from address
	    // And continues data handling
	 
	    std::ofstream crash(s_reportUrl, std::ios_base::app);
	    crash << err_url.what() << std::endl;
	}
}

void Crawler::crawlResource()
{
	using namespace std::chrono_literals;

	TagParser parser;

	std::size_t i = 0;

	while (m_internalLinks.size())
	{
		// send on output device
		m_viewerDevice->viewInfo(m_host.host() + m_internalLinks.front(),
			m_externalLinks.size(),
			m_internalLinks.size(),
			m_indexedInternal.size());

		// build request
		m_request.setHttpVersion(HttpRequest::Version::V_1_1);
		m_request.setRelativePath(m_internalLinks.front());
		m_request.setConnectionType(HttpRequest::ConnectionType::Close);

		// stores the indexed link
		m_indexedInternal.push_back(m_internalLinks.front());
		m_internalLinks.pop_front();

		try
		{
			// wait enter and in the time this is pause between http requests
			std::this_thread::sleep_for(std::chrono::milliseconds(m_timePause));

			m_request.build();
			m_response = m_controller.openUrl(m_request);

			if (m_response->isValid())
			{
				if (m_response->is_301_MovedPermanently() || m_response->is_302_MovedTemporarily())
				{
					// read header Location
					std::string tmpRefer;
					m_response->getHeader("location", tmpRefer);

					// test link
					Url url(tmpRefer);

					// until HttpLib don't support https
					if (m_host.protocol() == "https://")
					{
						m_viewerDevice->viewWarning("****************************************************\n"
							"* To address this need support Https.              *\n"
							"* But HttpLib still does not support this protocol.*\n"
							"****************************************************\n");
						continue;
					}

					// if it same host
					if (url.compareHost(m_host))
					{
						storeLinkToQueue(url, Internal);
					}
					else
					{
						storeLinkToQueue(url, External);
					}

					m_viewerDevice->viewResponseStatus(m_response->getCodeOfResponse());
				}
				else
				{
					// saves unique links
					m_viewerDevice->viewResponseStatus(m_response->getCodeOfResponse());

					parser.parseTags(m_response->getEntityBody(), "a");

					addToCrawlQueue(parser, Url(m_indexedInternal.back()));
				}
			}
			else
			{
				m_viewerDevice->viewWarning("Response of server invalid\n");
			}

		}
		catch (HttpErrorException const& fh)
		{
			m_viewerDevice->viewWarning(fh.what() + '\n');
			std::ofstream fails("failConnections.log", std::ios_base::app);

			fails << "Address: " << m_request.host() + m_request.relativePath()
				<< std::endl << fh.what() << std::endl;
		}
		catch(UrlParseErrorException const& err_url)
		{
		    // If failed parse link and object of Link throws exception
		    // We create report consisting from address
		    // And continues data handling
		 
		    std::ofstream crash(s_reportUrl, std::ios_base::app);

		    crash << err_url.what() << std::endl;

		    m_viewerDevice->viewWarning("An unidentified url. Results recorded in the log file: " + s_reportUrl + "\n");
		}

		// if specified max depth
		if (getMaxDepth())
		{
			++i;

			if (i < getMaxDepth())
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}

	// results of crawl website
	std::ofstream external("external_links.log");
	std::ofstream internal("internal_links.log");

	for (const auto &item : m_externalLinks)
	{
		external << item << std::endl;
	}

	for (const auto &item : m_indexedInternal)
	{
		internal << item << std::endl;
	}

	// for avoid double crawling
	if (m_infiniteCrawl)
	{
		// save addresses of web sites which already crawled
		m_indexedExternal.push_back(m_host.host());

		if (!m_externalLinks.empty())
		{
			m_request.clear();
			clearQueue(IndexedIn | Internal);

			m_host = m_externalLinks.front();
			m_externalLinks.pop_front();

			crawlStart();
		}
	}

	// reset user configurations after crawling
	resetConfigurations();
}

void Crawler::setStartAddress(const std::string &startAddr)
{
	m_host.parse(startAddr);	// <= test url

	// if link is valid
	if (m_host.isAbsoluteAddress())
	{
		if (m_viewerDevice != nullptr)
		{
			m_readyForStart = true;
		}
	}
}


void Crawler::setSignature(const std::string &signature)
{
	m_signature = m_sign + signature;
	m_request.setUserAgent(m_signature);
}


void Crawler::setViewer(const ViewHandler::CrawlerViewer *viewer)
{
	m_viewerDevice = viewer;
}


void Crawler::setPause(std::size_t timePause)
{
	m_timePause = timePause;
}


void Crawler::setMaxDepth(std::size_t depth)
{
	m_maxDepth = depth;
}


void Crawler::setModeInfiniteCrawl(bool val)
{
	m_infiniteCrawl = val;
}


void Crawler::setPathToSavePages(const std::string &path)
{
	m_pathToSave = path;
}

bool Crawler::readyForStart()
{
	return m_readyForStart;
}

void Crawler::start()
{
	if (readyForStart())
	{
		crawlStart();
	}
}

void Crawler::clearInternalQueue()
{
	clearQueue(CrawlerImpl::Crawler::Internal);
}

void Crawler::clearExternalQueue()
{
	clearQueue(CrawlerImpl::Crawler::External);
}

void Crawler::clearIndexedInQueue()
{
	clearQueue(CrawlerImpl::Crawler::IndexedIn);
}

void Crawler::clearIndexedExQueue()
{
	clearQueue(CrawlerImpl::Crawler::IndexedEx);
}

std::size_t Crawler::getTimePause() const
{
	return m_timePause;
}


std::size_t Crawler::getMaxDepth() const
{
	return m_maxDepth;
}


bool Crawler::getModeInfiniteCrawl() const
{
	return m_infiniteCrawl;
}


const std::string& Crawler::getSignature() const
{
	return m_signature;
}


const std::string& Crawler::getStartAddress() const
{
	return m_host.host();
}


const std::string& Crawler::getPathToSavePages() const
{
	return m_pathToSave;
}

void Crawler::resetConfigurations()
{
	m_host.clear();
	m_readyForStart = false;
	m_request.clear();
	m_timePause = STD_PAUSE_BETWEEN_REQUESTS;
	clearQueue(IndexedIn | Internal);
}

}