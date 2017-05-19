#include "crawler_controller.h"
#include "crawler_model.h"
#include "crawler_settings.h"
#include "http_connection.h"
#include "http_request.h"
#include "http_response.h"
#include "common.h"

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

void CrawlerController::startCrawling(const std::atomic_bool& stopCrawling)
{
	TagParser tagParser;
	HttpConnection httpConnection;
	HttpRequest request;
	const HttpResponse* response;

	model()->storeUrl(Url("/"), CrawlerModel::InternalUrlQueue);

	request.setHost(settings()->startUrlAddress().host());
	request.setHttpVersion(HttpRequest::Version::Version1_1);
	request.setConnectionType(HttpRequest::ConnectionType::Close);

	while (!stopCrawling)
	{
		if (model()->queue(CrawlerModel::InternalUrlQueue)->empty())
		{
			break;
		}

		Url url = model()->queue(CrawlerModel::InternalUrlQueue)->front();
		model()->storeUrl(url, CrawlerModel::InternalCrawledUrlQueue);
		model()->queue(CrawlerModel::InternalUrlQueue)->pop_front();

		std::this_thread::sleep_for(settings()->requestPause());

		request.setRelativePath(url.relativePath());
		request.build();

		try
		{
			response = httpConnection.openUrl(request);
		}
		catch (const HttpLib::HttpErrorException&)
		{
			if (!model()->size(CrawlerModel::InternalUrlQueue))
			{
				sendMessage(DNSErrorMessage{});
				break;
			}
		}

		if (!response->isValid())
		{
			sendMessage(WarningMessage{ "Invalid response of server" });
			continue;
		}

		if (response->is_301_MovedPermanently() ||
			response->is_302_MovedTemporarily())
		{
			handleRedirection(response, request);

			sendMessage(UrlMessage{ settings()->startUrlAddress().host() + url.relativePath(), std::string(), std::string(),
				response->responseCode(), CrawlerModel::InternalCrawledUrlQueue });
		}
		else
		{
			tagParser.parseTags(response->entityBody(), "a");

			//
			// TODO: remove from tagParser all HTTPS urls
			//

			model()->saveUniqueUrls(tagParser, settings()->startUrlAddress(), url);

			//
			// Find title tag
			//

			tagParser.parseTagsWithValues(response->entityBody(), "title");

			std::string title;

			if (!tagParser.size() || (tagParser.size() && tagParser[0].value().empty()))
			{
				sendMessage(EmptyTitleMessage{ url.host() + url.relativePath() });
			}
			else
			{
				title = tagParser[0].value();

				model()->addTitle(url, title);

				if (model()->isDuplicatedTitle(url, title))
				{
					if (model()->duplicatesTitle(url, title) == 2)
					{
						sendMessage(DuplicatedTitleMessage{ 
							settings()->startUrlAddress().host() + model()->firstDuplicatedTitle(title),
							title
						});
					}

					sendMessage(DuplicatedTitleMessage{ settings()->startUrlAddress().host() + url.relativePath(), title });
				}
			}

			//
			// Find meta description
			//

			std::string destricption;

			tagParser.parseTags(response->entityBody(), "meta");

			if (tagParser.size())
			{
				auto findDescription = [](const Tag& tag)
				{
					return Common::strToLower(tag.attribute("name")) == "description";
				};

				auto iter = std::find_if(std::begin(tagParser), std::end(tagParser), findDescription);

				if (iter != std::end(tagParser))
				{
					destricption = iter->attribute("content");

					if (!destricption.empty())
					{
						model()->addDescription(url, destricption);

						if (model()->isDuplicatedDescription(url, destricption))
						{
							if (model()->duplicatesDescription(url, destricption) == 2)
							{
								sendMessage(DuplicatedDescriptionMessage{
									settings()->startUrlAddress().host() + model()->firstDuplicatedDescription(destricption),
									iter->attribute("content")
								});
							}

							sendMessage(DuplicatedDescriptionMessage{ settings()->startUrlAddress().host() + url.relativePath(), destricption });
						}
					}
					else
					{
						sendMessage(EmptyDescriptionMessage{ url.host() + url.relativePath() });
					}
				}
			}

			sendMessage(UrlMessage{ settings()->startUrlAddress().host() + url.relativePath(), title, destricption,
				response->responseCode(), CrawlerModel::InternalCrawledUrlQueue });
		}

		sendMessage(QueueSizeMessage{ CrawlerModel::InternalUrlQueue | CrawlerModel::InternalCrawledUrlQueue, 
			model()->size(CrawlerModel::InternalUrlQueue) + model()->size(CrawlerModel::InternalCrawledUrlQueue) });

		sendMessage(QueueSizeMessage{ CrawlerModel::InternalUrlQueue, model()->size(CrawlerModel::InternalUrlQueue) });
		sendMessage(QueueSizeMessage{ CrawlerModel::InternalCrawledUrlQueue, model()->size(CrawlerModel::InternalCrawledUrlQueue) });
		sendMessage(QueueSizeMessage{ CrawlerModel::ExternalUrlQueue, model()->size(CrawlerModel::ExternalUrlQueue) });
	}

	sendMessage(WarningMessage{ "Crawling ended up!" });
}

void CrawlerController::handleRedirection(const HttpLib::HttpResponse* response, HttpLib::HttpRequest& request)
{
	std::string urlFromLocationHeader;
	response->header("location", urlFromLocationHeader);

	Url url(urlFromLocationHeader);

	if (url.protocol() == "https://")
	{
		sendMessage(WarningMessage{ "Current HTTP library does not support HTTPS protocol" });
		return;
	}

	if (url.compareHost(settings()->startUrlAddress()) || url.host().empty())
	{
		if (!model()->isItemExistsIn(url, CrawlerModel::InternalCrawledUrlQueue))
		{
			model()->storeUrl(url, CrawlerModel::InternalUrlQueue);
		}
	}
	else
	{
		settings()->setHost(url);
		request.setHost(url.host());

		if (!model()->isItemExistsIn(url, CrawlerModel::InternalCrawledUrlQueue))
		{
			model()->storeUrl(url, CrawlerModel::InternalUrlQueue);
		}
	}
}

}