#include "stdlibs.h"
#include "http_response.h"
#include "console_crawler_viewer.h"
#include "crawler_model.h"

void ViewHandler::ConsoleCrawlerViewer::viewInfo(const std::string &url,
                                          size_type extl,
                                          size_type intl,
                                          size_type indexed) const
{
    std::cout << std::endl << url << "\n";
    std::cout << "Found " << extl << " external links.\n";
    std::cout << "Passed " << indexed << " links.\n";
    std::cout << "In queue for crawl " << intl << " internal links.\n";
}


void ViewHandler::ConsoleCrawlerViewer::viewCurrentRequestedUrl(const std::string& url) const
{
	std::cout << std::endl << url << "\n";
}


void ViewHandler::ConsoleCrawlerViewer::viewInternalUrlsSize(size_type n) const
{
	std::cout << "In queue for crawl " << n << " internal links.\n";
}


void ViewHandler::ConsoleCrawlerViewer::viewExternalUrlsSize(size_type n) const
{
	std::cout << "Found " << n << " external links.\n";
}


void ViewHandler::ConsoleCrawlerViewer::viewInternalCrawlerUrlsSize(size_type n) const
{
	std::cout << "Passed " << n << " links.\n";
}

void ViewHandler::ConsoleCrawlerViewer::viewResponseStatus(int code) const
{
    if(code == HttpLib::HttpResponse::ResponseCode::Continue100)
    {
        std::cout << "Response of server: 100 Continue" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::SwitchingProtocol101)
    {
        std::cout << "Response of server: 101 Switching Protocols" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::Ok200)
    {
        std::cout << "Response of server: 200 OK" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::Created201)
    {
        std::cout << "Response of server: 201 Created" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::Accepted202)
    {
        std::cout << "Response of server: 202 Accepted" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::NonAuthoritativeInformation203)
    {
        std::cout << "Response of server: 203 Non Authoritative Information" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::NoContent204)
    {
        std::cout << "Response of server: 204 No Content" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::ResetContent205)
    {
        std::cout << "Response of server: 205 Reset Content" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::PartialContent206)
    {
        std::cout << "Response of server: 206 Partial Content" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::MultipleChoices300)
    {
        std::cout << "Response of server: 300 Multiple Choices" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::MovedPermanently301)
    {
        std::cout << "Response of server: 301 Moved Permanently" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::MovedTemporarily302)
    {
        std::cout << "Response of server: 302 Moved Temporarily" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::SeeOther303)
    {
        std::cout << "Response of server: 303 See Other" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::NotModified304)
    {
        std::cout << "Response of server: 304 Not Modified" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::UseProxy305)
    {
        std::cout << "Response of server: 305 Use Proxy" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::BadRequest400)
    {
        std::cout << "Response of server: 400 Bad Request" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::Unauthorized401)
    {
        std::cout << "Response of server: 401 Unauthorized" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::PaymentRequired402)
    {
        std::cout << "Response of server: 402 Payment Required" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::Forbidden403)
    {
        std::cout << "Response of server: 403 Forbidden" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::NotFound404)
    {
        std::cout << "Response of server: 404 Not Found" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::MethodNotAllowed405)
    {
        std::cout << "Response of server: 405 Method Not Allowed" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::NonAcceptable406)
    {
        std::cout << "Response of server: 406 Non Acceptable" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::ProxyAuthenticationRequired407)
    {
        std::cout << "Response of server: 407 Proxy Authentication Required" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::RequestTimeout408)
    {
        std::cout << "Response of server: 408 Request Timeout" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::Conflict409)
    {
        std::cout << "Response of server: 409 Conflict" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::Gone410)
    {
        std::cout << "Response of server: 410 Gone" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::LengthRequired)
    {
        std::cout << "Response of server: 411 Length Required" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::PreconditionFailed412)
    {
        std::cout << "Response of server: 412 Precondition Failed" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::RequestEntityTooLarge413)
    {
        std::cout << "Response of server: 413 Request Entity Too Large" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::RequestUriTooLong414)
    {
        std::cout << "Response of server: 414 Request URI Too Long" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::UnsupportedMediaType415)
    {
        std::cout << "Response of server: 415 Unsupported Media Type" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::InternalServerError500)
    {
        std::cout << "Response of server: 500 Internal Server Error" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::NotImplemented501)
    {
        std::cout << "Response of server: 501 Not Implemented" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::BadGateway502)
    {
        std::cout << "Response of server: 502 Bad Gateway" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::ServiceUnavailable503)
    {
        std::cout << "Response of server: 503 Service Unavailable" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::GatewayTimeout504)
    {
        std::cout << "Response of server: 504 Gateway Timeout" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::ResponseCode::HttpVersionNotSupported505)
    {
        std::cout << "Response of server: 505 HTTP Version Not Supported" << std::endl;
        return;
    }

    std::cout << "Undefined response of server" << std::endl;
}

void ViewHandler::ConsoleCrawlerViewer::viewWarning(const std::string &warning) const
{
	std::cout << warning << std::endl;
}

void ViewHandler::ConsoleCrawlerViewer::receiveMessage(const CrawlerImpl::IMessage& message)
{
	using namespace CrawlerImpl;

	switch (message.type())
	{
		case IMessage::MessageType::CurrentRequestedUrl:
		{
			const CurrentRequestedUrlMessage& actualMessageType =
				static_cast<const CurrentRequestedUrlMessage&>(message);

			viewCurrentRequestedUrl(actualMessageType.url());

			break;
		}

		case IMessage::MessageType::CurrentQueueSize:
		{
			const QueueSizeMessage& actualMessageType = 
				static_cast<const QueueSizeMessage&>(message);

			assert(actualMessageType.queueType() == CrawlerModel::InternalUrlQueue ||
				actualMessageType.queueType() == CrawlerModel::InternalCrawledUrlQueue ||
				actualMessageType.queueType() == CrawlerModel::ExternalUrlQueue ||
				actualMessageType.queueType() == CrawlerModel::ExternalCrawledUrlQueue
			);

			if (actualMessageType.queueType() == CrawlerModel::InternalUrlQueue)
			{
				viewInternalUrlsSize(actualMessageType.size());
			}

			if (actualMessageType.queueType() == CrawlerModel::InternalCrawledUrlQueue)
			{
				viewInternalCrawlerUrlsSize(actualMessageType.size());
			}

			if (actualMessageType.queueType() == CrawlerModel::ExternalUrlQueue)
			{
				viewExternalUrlsSize(actualMessageType.size());
			}

			break;
		}

		case IMessage::MessageType::HttpResponseCode:
		{
			const HttpResponseCodeMessage& actualMessageType =
				static_cast<const HttpResponseCodeMessage&>(message);

			viewResponseStatus(actualMessageType.code());

			break;
		}

		case IMessage::MessageType::WarningType:
		{
			const WarningMessage& actualMessageType =
				static_cast<const WarningMessage&>(message);

			viewWarning(actualMessageType.warning());

			break;
		}
	}
}
