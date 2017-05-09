#pragma once

#include "stdlibs.h"
#include "imessage_receiver.h"
#include "message_types.h"

namespace WebSiteAnalyzer
{

class MessageMapper 
	: public QObject
	, public CrawlerImpl::IMessageReceiver
{
	Q_OBJECT

public:
	Q_SIGNAL void signal_modelUpdated();
	Q_SIGNAL void signal_warningMessage(const std::string&);
	Q_SIGNAL void signal_crawlerStopped();

protected:
	virtual void receiveMessage(const CrawlerImpl::IMessage& message) override;
};

}
