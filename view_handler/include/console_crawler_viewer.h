#pragma once

#include "crawler_viewer.h"
#include "imessage_receiver.h"

namespace ViewHandler
{

class ConsoleCrawlerViewer
    : public CrawlerViewer
{
    void viewInfo(const std::string &url,
                  size_type extl,
                  size_type intl,
                  size_type indexed) const;

	void viewCurrentRequestedUrl(const std::string& url) const;

	void viewInternalUrlsSize(size_type n) const;
	void viewExternalUrlsSize(size_type n) const;
	void viewInternalCrawlerUrlsSize(size_type n) const;

    void viewResponseStatus(int code) const;
	void viewWarning(const std::string &warning) const;

protected:
	virtual void receiveMessage(const CrawlerImpl::IMessage& message) override;
};

}
