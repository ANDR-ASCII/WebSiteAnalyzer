#include "stdlibs.h"
#include "general_helper_functions.h"
#include "crawler.h"
#include "tag_parser.h"
#include "attribute.h"

// correct time
#define STD_PAUSE_BETWEEN_REQUESTS 3000

const std::string CrawlerImpl::Crawler::s_storage_of_phrases = "phrases.txt";
const std::string CrawlerImpl::Crawler::s_report_url = "url_crash_report.log";
const std::string CrawlerImpl::Crawler::s_storage_urls = "table_of_addresses.ta";

CrawlerImpl::Crawler::Crawler()
	: m_timePause(STD_PAUSE_BETWEEN_REQUESTS)
	, m_sign("PasBot v1.0 Alpha ")
    , m_maxDepth(0)
    , m_readyForStart(false)
    , m_infiniteCrawl(false)
{
    m_signature = m_sign;
	m_request.setUserAgent(m_signature);
}

CrawlerImpl::Crawler::Crawler(const std::string &startAddr)
    : m_timePause(STD_PAUSE_BETWEEN_REQUESTS)
    , m_sign("PasBot v1.0 Alpha ")
    , m_maxDepth(0)
    , m_infiniteCrawl(false)
{
	m_signature = m_sign;
	setStartAddress(startAddr);
	m_request.setUserAgent(m_signature);
}

CrawlerImpl::Crawler::Crawler(const std::string &startAddr, std::size_t timePause)
    : m_timePause(timePause)
    , m_sign("PasBot v1.0 Alpha ")
    , m_maxDepth(0)
    , m_infiniteCrawl(false)
{
    m_signature = m_sign;
	setStartAddress(startAddr);
	m_request.setUserAgent(m_signature);
}

void CrawlerImpl::Crawler::crawlStart()
{
	HtmlParser::TagParser parser;

    // send on output device
    m_viewerDevice->viewInfo(m_host.host(),
                            m_externalLinks.size(),
                            m_internalLinks.size(),
                            m_indexedInternal.size());

    // set configurations of request
	m_request.setHost(m_host.host());
	m_request.setHttpVersion(HttpLib::HttpRequest::Version::V_1_1);
	m_request.setConnectionType(HttpLib::HttpRequest::ConnectionType::Close);

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
            while(m_response->is_301_MovedPermanently() || m_response->is_302_MovedTemporarily())
            {
                m_viewerDevice->viewResponseStatus(m_response->getCodeOfResponse());
                addLinkToQueue(m_host, IndexedEx); // save current url

                // set flag
                mpOccur = !mpOccur;

                std::string tmpRefer, firstHost = m_host.host();
                m_response->getHeader("location", tmpRefer);
                m_host.parse(tmpRefer);

                // until HttpLib don't support https
                if(m_host.protocol() == "https://")
                {

                    m_viewerDevice->viewWarning("****************************************************\n"
                                               "* To address this need support Https.              *\n"
                                               "* But HttpLib still does not support this protocol.*\n"
                                               "****************************************************\n");


                    // jump into crawlResource with empty parameters
                    // for the next crawl
                    if(getMaxDepth() > 1)
                    {
                        crawlResource();
                    }
                }

                if(m_host.isAbsoluteAddress())
                {
                    m_request.setHost(m_host.host());
                    m_request.setRelativePath(m_host.relativePath());
                }
                else if(m_host.isRelativeAddress())
                {
                    // in order not to lose the host
                    m_host = firstHost + convertRelativeAddr(m_host.relativePath(), "/");
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
            addToQueue(parser, m_indexedInternal.back());

            if(getMaxDepth() == 1)
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
    catch(HttpLib::exFailHttp &fh)
    {
        std::cout << "Web site unavailable.\n";
        std::ofstream fails("failConnections.log", std::ios_base::app);
        fails << "Address: " << m_request.getHost() + m_request.getRelativePath()
                << std::endl << fh.what() << std::endl;

        if(getMaxDepth() > 1)
        {
            crawlResource();
        }
        else
        {
            return;
        }
    }
//     catch(HtmlParser::exErrorUrl& err_url)
//     {
//         // If failed parse link and object of Link throws exception
//         // We create report consisting from address
//         // And continues data handling
// 
//         std::ofstream crash(s_report_url, std::ios_base::app);
//         crash << err_url.what() << std::endl;
//     }
}

void CrawlerImpl::Crawler::crawlResource()
{
	using namespace std::chrono_literals;

	HtmlParser::TagParser parser;

    std::size_t i = 0;

	while (m_internalLinks.size())
	{
	    // send on output device
        m_viewerDevice->viewInfo(m_host.host() + m_internalLinks.front(),
                                m_externalLinks.size(),
                                m_internalLinks.size(),
                                m_indexedInternal.size());

		// build request
		m_request.setHttpVersion(HttpLib::HttpRequest::Version::V_1_1);
		m_request.setRelativePath(m_internalLinks.front());
		m_request.setConnectionType(HttpLib::HttpRequest::ConnectionType::Close);

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
					HtmlParser::Url url(tmpRefer);

                    // until HttpLib don't support https
                    if(m_host.protocol() == "https://")
                    {
                        m_viewerDevice->viewWarning("****************************************************\n"
                                                   "* To address this need support Https.              *\n"
                                                   "* But HttpLib still does not support this protocol.*\n"
                                                   "****************************************************\n");
                        continue;
                    }

                    // if it same host
                    if(url.compareHost(m_host))
                    {
                        addLinkToQueue(url, CrawlerImpl::Crawler::Internal);
                    }
                    else
                    {
                        addLinkToQueue(url, CrawlerImpl::Crawler::External);
                    }

                    m_viewerDevice->viewResponseStatus(m_response->getCodeOfResponse());
                }
                else
                {
                    // saves unique links
                    m_viewerDevice->viewResponseStatus(m_response->getCodeOfResponse());

                    parser.parseTags(m_response->getEntityBody(), "a");
                    addToQueue(parser, HtmlParser::Url(m_indexedInternal.back()));
                }
            }
            else
            {
                m_viewerDevice->viewWarning("Response of server invalid\n");
            }

        }
        catch(HttpLib::exFailHttp &fh)
        {
            m_viewerDevice->viewWarning(fh.what() + '\n');
            std::ofstream fails("failConnections.log", std::ios_base::app);
            fails << "Address: " << m_request.getHost() + m_request.getRelativePath()
                    << std::endl << fh.what() << std::endl;
        }
//         catch(HtmlParser::exErrorUrl &err_url)
//         {
//             // If failed parse link and object of Link throws exception
//             // We create report consisting from address
//             // And continues data handling
// 
//             std::ofstream crash(s_report_url, std::ios_base::app);
//             crash << err_url.what() << std::endl;
//             m_viewerDevice->viewWarning("An unidentified url. Results recorded in the log file: " + s_report_url + "\n");
//         }

        // if specified max depth
        if(getMaxDepth())
        {
            ++i;

            if(i < getMaxDepth())
            {
                continue;
            }
            else
            {
                break;
            }
        }
	}

    /** ############################################################# **/

    // results of crawl website
	std::ofstream external("external_links.log");
	std::ofstream internal("internal_links.log");

	for(const auto &item : m_externalLinks)
    {
        external << item << std::endl;
    }

    for(const auto &item : m_indexedInternal)
    {
        internal << item << std::endl;
    }

    /** ############################################################# **/


    /** check infinite crawling **/


    // for avoid double crawling
    if(m_infiniteCrawl)
    {
        // save addresses of web sites which already crawled
        m_indexedExternal.push_back(m_host.host());

        if(!m_externalLinks.empty())
        {
            m_request.clear();
            clearQueue(CrawlerImpl::Crawler::IndexedIn | CrawlerImpl::Crawler::Internal);

            m_host = m_externalLinks.front();
            m_externalLinks.pop_front();

            crawlStart();
        }
    }

    // reset user configurations after crawling
	configByDefault();
}

// second argument expect relative address with specified name file or without it
void CrawlerImpl::Crawler::addToQueue(const HtmlParser::TagParser &parser,
                                      const HtmlParser::Url &relativePath, Crawler::UrlType urls)
{
	HtmlParser::Url analyzeUrl;
    std::string resultAddr;

    for (const auto &tag : parser)
    {
	    try
	    {
            // check link
            analyzeUrl.parse(tag.attribute("href"));

            // skip undefined files
            if(!analyzeUrl.file().empty()
               && analyzeUrl.fileType() != HtmlParser::Url::FileType::ExecutableWebFile)
            {
                continue;
            }

            if(analyzeUrl.isAbsoluteAddress())
            {
                if (!analyzeUrl.compareHost(m_host)
                    && (urls == UrlType::Default || urls == UrlType::External))
                {
                    // found external link
                    if(!existsInQueues(analyzeUrl, CrawlerImpl::Crawler::External))
                    {
                        // temporary compare
                        // while HttpLib don't support https
                        if(analyzeUrl.protocol() != "https://")
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

            if(analyzeUrl.isAnchor())
            {
                continue;
            }

            /**
            ** WARNING
            ** There is a possibility that there will be 2 identical addresses: / and /index.php often equivalent
            **/

            /** Second part. Handling relative path. **/

            // can throw std::runtime_error
            resultAddr = convertRelativeAddr(analyzeUrl, relativePath);

            if(!existsInQueues(resultAddr)
                && (urls == UrlType::Internal || urls == UrlType::Default))
            {
                m_internalLinks.push_back(resultAddr);
            }

        }
		catch (...)
		{

		}
//         catch(HtmlParser::exErrorUrl &err_url)
//         {
//             // If failed parse link and object of Link throws exception
//             // We create report consisting from address
//             // And continues data handling
// 
//             std::ofstream crash(s_report_url, std::ios_base::app);
//             crash << err_url.what() << std::endl;
//         }
    }
}

// true if found specified link in specified queue, otherwise false
bool CrawlerImpl::Crawler::existsInQueues(const HtmlParser::Url &url, int whereToSearch)
{
    if(whereToSearch == CrawlerImpl::Crawler::Internal)
    {
        for(const auto &item : m_indexedInternal)
        {
            if (item == url.relativePath())
            {
                return true;
            }
        }

        for (const auto &item : m_internalLinks)
        {
            if (item == url.relativePath())
            {
                return true;
            }
        }

        return false;
    }

    if(whereToSearch == CrawlerImpl::Crawler::External)
    {
        for (const auto &item : m_indexedExternal)
        {
            // compare with full address
            if(url.compareHost(item))
            {
                return true;
            }
        }

        for (const auto &item : m_externalLinks)
        {
            // compare with full address
            if(url.compareHost(item))
            {
                return true;
            }
        }
    }

	return false;
}

void CrawlerImpl::Crawler::setStartAddress(const std::string &startAddr)
{
	m_host.parse(startAddr);	// <= test url

	// if link is valid
	if (m_host.isAbsoluteAddress())
	{
	    if(m_viewerDevice != nullptr)
		{
		    m_readyForStart = true;
		}
	}
}


void CrawlerImpl::Crawler::setSignature(const std::string &signature)
{
	m_signature = m_sign + signature;
	m_request.setUserAgent(m_signature);
}


void CrawlerImpl::Crawler::setViewer(const ViewHandler::CrawlerViewer *viewer)
{
	m_viewerDevice = viewer;
}


void CrawlerImpl::Crawler::setPause(std::size_t timePause)
{
	m_timePause = timePause;
}


void CrawlerImpl::Crawler::setMaxDepth(std::size_t depth)
{
	m_maxDepth = depth;
}


void CrawlerImpl::Crawler::setModeInfiniteCrawl(bool val)
{
	m_infiniteCrawl = val;
}


void CrawlerImpl::Crawler::setPathToSavePages(const std::string &path)
{
	m_pathToSave = path;
}

bool CrawlerImpl::Crawler::readyForStart()
{
	return m_readyForStart;
}

void CrawlerImpl::Crawler::start()
{
	if (readyForStart())
	{
		crawlStart();
	}
}

void CrawlerImpl::Crawler::clearQueue(int queueMark)
{
    if((queueMark & CrawlerImpl::Crawler::Internal) == CrawlerImpl::Crawler::Internal)
    {
        m_internalLinks.clear();
    }
    if((queueMark & CrawlerImpl::Crawler::IndexedIn) == CrawlerImpl::Crawler::IndexedIn)
    {
        m_indexedInternal.clear();
    }
    if((queueMark & CrawlerImpl::Crawler::External) == CrawlerImpl::Crawler::External)
    {
        m_externalLinks.clear();
    }
    if((queueMark & CrawlerImpl::Crawler::IndexedEx) == CrawlerImpl::Crawler::IndexedEx)
    {
        m_indexedExternal.clear();
    }
}

void CrawlerImpl::Crawler::clearInternalQueue()
{
    clearQueue(CrawlerImpl::Crawler::Internal);
}

void CrawlerImpl::Crawler::clearExternalQueue()
{
    clearQueue(CrawlerImpl::Crawler::External);
}

void CrawlerImpl::Crawler::clearIndexedInQueue()
{
    clearQueue(CrawlerImpl::Crawler::IndexedIn);
}

void CrawlerImpl::Crawler::clearIndexedExQueue()
{
    clearQueue(CrawlerImpl::Crawler::IndexedEx);
}

std::size_t CrawlerImpl::Crawler::getTimePause() const
{
	return m_timePause;
}


std::size_t CrawlerImpl::Crawler::getMaxDepth() const
{
	return m_maxDepth;
}


bool CrawlerImpl::Crawler::getModeInfiniteCrawl() const
{
	return m_infiniteCrawl;
}


const std::string& CrawlerImpl::Crawler::getSignature() const
{
	return m_signature;
}


const std::string& CrawlerImpl::Crawler::getStartAddress() const
{
	return m_host.host();
}


const std::string& CrawlerImpl::Crawler::getPathToSavePages() const
{
	return m_pathToSave;
}

void CrawlerImpl::Crawler::configByDefault()
{
    m_host.clear();
    m_readyForStart = false;
    m_request.clear();
    m_timePause = STD_PAUSE_BETWEEN_REQUESTS;
    clearQueue(CrawlerImpl::Crawler::IndexedIn | CrawlerImpl::Crawler::Internal);
}

// add link into specified queue with check on uniqueness
// if addition into internal queue then adds only relative path.
// if addition into external queue then adds url.getMainAddress()
void CrawlerImpl::Crawler::addLinkToQueue(const HtmlParser::Url &url, int whereToSearch)
{
    if(whereToSearch == CrawlerImpl::Crawler::Internal)
    {
        if(!existsInQueues(url))
        {
            m_internalLinks.push_back(url.relativePath());
        }
        return;
    }

    if(whereToSearch == CrawlerImpl::Crawler::IndexedIn)
    {
        if(!existsInQueues(url))
        {
            m_indexedInternal.push_back(url.relativePath());
        }
        return;
    }

    if(whereToSearch == CrawlerImpl::Crawler::External)
    {
        if(!existsInQueues(url, whereToSearch))
        {
            m_externalLinks.push_back(url.host());
        }
        return;
    }

    if(whereToSearch == CrawlerImpl::Crawler::IndexedEx)
    {
        if(!existsInQueues(url, whereToSearch))
        {
            m_indexedExternal.push_back(url.host());
        }
        return;
    }
}

/** *********************** API FOR CONVERSATION TRUE RELATIVE PATHES ************************************************ **/

// converts a relative path in accordance with the place where the received address
std::string CrawlerImpl::Crawler::convertRelativeAddr(const HtmlParser::Url &relAddr, const HtmlParser::Url &where)
{
    // if path specified beginning slash
    // it means path specified relative by root of website
    // returns it
    if(relAddr.relativePath()[0] == '/')
    {
        return relAddr.relativePath();
    }

    if(relAddr.relativePath().empty() && where.relativeDir().empty())
    {
        // root
        return "/";
    }

    if(relAddr.relativePath().empty())
    {
        return where.relativePath();
    }

    if(where.relativeDir().empty())
    {
        return relAddr.relativePath();
    }

    /** Handling **/
    std::vector<std::string> dirFirst;
    std::vector<std::string> dirSecond;

    if(!relAddr.relativeDir().empty())
    {
        dirFirst = dividePath(relAddr.relativeDir());
    }

    dirSecond = dividePath(where.relativeDir());

    std::size_t sizeFirst = dirFirst.size();
    std::size_t sizeSecond = dirSecond.size();

    std::size_t counter = 0;
    std::vector<std::size_t> indexDots; // consisting indexes of elements of dots in path ./folder/

    for(std::size_t i = 0; i < sizeFirst; ++i)
    {
        if(dirFirst[i] == "..")
        {
            ++counter;
        }

        if(dirFirst[i] == ".")
        {
            indexDots.push_back(i);
        }
    }

    std::string result;

    // delete as many folders as met ".."
    for(std::size_t i = 0; i < sizeSecond && i < counter; ++i)
    {
        dirSecond.pop_back();
    }

    // resize
    sizeSecond = dirSecond.size();

    // build parts of path, where received link
    for(std::size_t i = 0; i < sizeSecond; ++i)
    {
        result += dirSecond[i] + '/';
    }

    // build link
    for(auto i = counter; i < sizeFirst; ++i)
    {
        if(!General::findValueInVector(indexDots, i))
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
std::vector<std::string> CrawlerImpl::Crawler::dividePath(const std::string &path)
{
    if(path.empty())
    {
        return std::vector<std::string>();
    }

    std::vector<std::string> dirs;
    std::string::size_type pointer = path.find('/');

    if(pointer != std::string::npos)
    {
        std::string tmpFolder;
        std::string::size_type pos = 0;

        do
        {
            tmpFolder.assign(path, pos, !pos ? pointer : pointer - pos);
            dirs.push_back(tmpFolder);
            pos = pointer + 1;
        }
        while( (pointer = path.find('/', pos)) != std::string::npos);
    }

    return dirs;
}
