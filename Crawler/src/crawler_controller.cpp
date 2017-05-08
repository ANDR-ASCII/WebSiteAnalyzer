#include "crawler_controller.h"
#include "crawler_model.h"
#include "crawler_settings.h"
#include "http_connection.h"
#include "http_request.h"
#include "http_response.h"

namespace CrawlerImpl
{

using namespace HtmlParser;
using namespace HttpLib;


void CrawlerController::setModel(CrawlerModel* model)
{
	m_model = model;
}

void CrawlerController::setSettings(CrawlerSettings* settings)
{
	m_settings = settings;
}

CrawlerImpl::CrawlerModel* CrawlerController::model() const
{
	return m_model;
}

CrawlerImpl::CrawlerSettings* CrawlerController::settings() const
{
	return m_settings;
}

void CrawlerController::startCrawling()
{
	TagParser tagParser;
	HttpConnection httpConnection;
	HttpRequest request;
	const HttpResponse* response;

	model()->storeUrl(Url("/"), CrawlerModel::InternalUrlQueue);

	request.setHost(settings()->startUrlAddress().host());
	request.setHttpVersion(HttpRequest::Version::Version1_1);
	request.setConnectionType(HttpRequest::ConnectionType::Close);

	while (true)
	{
		if (model()->queue(CrawlerModel::InternalUrlQueue)->empty())
		{
			return;
		}

		Url url = model()->queue(CrawlerModel::InternalUrlQueue)->front();

		model()->queue(CrawlerModel::InternalCrawledUrlQueue)->push_back(url);
		model()->queue(CrawlerModel::InternalUrlQueue)->pop_front();

		std::this_thread::sleep_for(settings()->requestPause());

		sendMessage(CurrentRequestedUrlMessage{ settings()->startUrlAddress().host() + url.relativePath() });
		sendMessage(QueueSizeMessage{ CrawlerModel::InternalUrlQueue, model()->size(CrawlerModel::InternalUrlQueue) });
		sendMessage(QueueSizeMessage{ CrawlerModel::InternalCrawledUrlQueue, model()->size(CrawlerModel::InternalCrawledUrlQueue) });
		sendMessage(QueueSizeMessage{ CrawlerModel::ExternalUrlQueue, model()->size(CrawlerModel::ExternalUrlQueue) });

		request.setRelativePath(url.relativePath());
		request.build();
		response = httpConnection.openUrl(request);

		if (!response->isValid())
		{
			sendMessage(WarningMessage{ "Invalid response of server" });
			continue;
		}

		if (response->is_301_MovedPermanently() ||
			response->is_302_MovedTemporarily())
		{
			handleRedirection(response);
		}
		else
		{
			tagParser.parseTags(response->entityBody(), "a");

			//
			// TODO: remove from tagParser all HTTPS urls
			//

			model()->saveUniqueUrls(tagParser, settings()->startUrlAddress(), url);
		}

		sendMessage(HttpResponseCodeMessage{ response->responseCode() });
	}
}

void CrawlerController::handleRedirection(const HttpLib::HttpResponse* response)
{
	std::string urlFromLocationHeader;
	response->header("location", urlFromLocationHeader);

	Url url(urlFromLocationHeader);

	if (settings()->startUrlAddress().protocol() == "https://")
	{
		sendMessage(WarningMessage{ "Current HTTP library does not support HTTPS protocol" });
		return;
	}

	if (url.compareHost(settings()->startUrlAddress()))
	{
		model()->storeUrl(url, CrawlerModel::InternalUrlQueue);
	}
	else
	{
		model()->storeUrl(url, CrawlerModel::ExternalUrlQueue);
	}
}

}