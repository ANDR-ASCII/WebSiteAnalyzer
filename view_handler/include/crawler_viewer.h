#pragma once

#include "stdlibs.h"
#include "imessage_receiver.h"

namespace ViewHandler
{

class CrawlerViewer
	: public CrawlerImpl::IMessageReceiver
{
public:
    using size_type = std::deque<std::string>::size_type;

    virtual void viewInfo(const std::string &url, 
		size_type extl,
        size_type intl,
        size_type indexed) const = 0;

    virtual void viewResponseStatus(int code) const = 0;
    virtual void viewWarning(const std::string &warning) const = 0;
};

}
