#pragma once

#include "stdlibs.h"
#include "url.h"
#include "tag_parser.h"
#include "http_response.h"
#include "http_request.h"
#include "http_connection.h"
#include "crawler_viewer.h"
#include "crawler_base.h"

namespace CrawlerImpl
{
	using namespace HtmlParser;
	using namespace HttpLib;

    /**************************************************************************************************************
     *
     *   #1. Object of type Crawler saves in queue of internal links, only relative part of link.
     *   Likewise saves in indexed queue. The relative path URL must start with character slash "/".
     *
     **************************************************************************************************************
     *
     *   #2. Conversion functions relative addresses returns a new address, starting with the character slash "/".
     *
     **************************************************************************************************************
     *
     *   #3. Single slash - "/" in internal and indexed queues denotes the root of the site.
     *
     **************************************************************************************************************
     */


    class Crawler : public CrawlerBase
    {
    public:

        Crawler();
        Crawler(const std::string& startAddr);
        Crawler(const std::string& startAddr, std::size_t timePause);

        void setStartAddress(const std::string& startAddr);
		void setSignature(const std::string& signature);
		void setViewer(const ViewHandler::CrawlerViewer* viewer);
		void setPause(std::size_t timePause);
		void setMaxDepth(std::size_t depth = 0);
		void setModeInfiniteCrawl(bool val);
		void setPathToSavePages(const std::string& path);

        bool readyForStart();
        void start();

        void clearInternalQueue();
        void clearExternalQueue();
        void clearIndexedInQueue();
        void clearIndexedExQueue();

		std::size_t getTimePause() const;
		std::size_t getMaxDepth() const;
		bool getModeInfiniteCrawl() const; 
		const std::string& getSignature() const;
		const std::string& getStartAddress() const;
		const std::string& getPathToSavePages() const;

	private:

		enum QueueType 
		{ 
			Internal, 
			External = 2, 
			IndexedIn = 4, 
			IndexedEx = 8 
		};

		enum class UrlType 
		{ 
			Internal, 
			External, 
			Default 
		};

	private:
		void crawlStart();
		void crawlResource();

		// reset configurations with clear internal and indexedInternal queues
		void resetConfigurations();

	private:
		HttpResponse const* m_response;
		HttpRequest m_request;
		HttpConnection m_controller;

		// between requests
		std::size_t m_timePause;

		// full signature in User-agent
		std::string m_signature;

		// original signature of crawler
		std::string m_sign;
		std::size_t m_maxDepth;

		bool m_readyForStart;
		bool m_infiniteCrawl;

		std::string m_pathToSave;

		const ViewHandler::CrawlerViewer* m_viewerDevice = nullptr;
    };

}