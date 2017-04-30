#pragma once

#include "stdlibs.h"
#include "../view_handler/include/console_crawler_viewer.h"
#include "Crawler.h"
#include "url.h"

#define MEASURE_OF_TIME " milliseconds"

// correct time
#if PLATFORM == PLATFORM_WINDOWS

#define CLEAR_SCREEN "cls"

#else

#define CLEAR_SCREEN "clear"

#endif // PLATFORM

#define AMOUNT_HANDLERS 9

namespace UserInterface
{
    class Menu
    {
    public:
        Menu();

    private:

        void printMenu();
        void interrogation();

        // handlers
        void quit();
        void start();
        void clearScreen();
        void setStartAddress();
        void setPauseBetweenReqs();
        void setUserAgent();
        void setMaxDepth();
        void switchInfiniteMode();
        void setPathToSave();

        //======================================

        void (Menu::*runHandler[AMOUNT_HANDLERS])() =
        {
            &Menu::quit, &Menu::start, &Menu::clearScreen, &Menu::setStartAddress,
            &Menu::setPauseBetweenReqs, &Menu::setUserAgent, &Menu::setMaxDepth,
            &Menu::switchInfiniteMode, &Menu::setPathToSave
        };

        bool turnOnOffInfCrawl = false;
        bool turnOnOffSearchInSE = false;

        ViewHandler::ConsoleCrawlerViewer ccv;
        CrawlerImpl::Crawler crawl;

		HtmlParser::Url startLnk;

        static const boost::regex getNumbers;

        void eatline() { while(std::cin.get() != '\n'); }
        void explode(const std::string &str, std::vector<std::size_t> &result);
    };
}