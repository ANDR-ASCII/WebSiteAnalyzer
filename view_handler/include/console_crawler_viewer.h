#pragma once

#include "crawler_viewer.h"

namespace ViewHandler
{

class ConsoleCrawlerViewer
    : public CrawlerViewer
{
    virtual void viewInfo(const std::string &url,
                  size_type extl,
                  size_type intl,
                  size_type indexed) const;

    virtual void viewResponseStatus(int code) const;
    virtual void viewWarning(const std::string &warning) const;
};

}
