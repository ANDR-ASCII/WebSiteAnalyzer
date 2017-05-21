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

			//
			// TODO: need to handle timeout
			//
		}

		if (!response->isValid())
		{
			sendMessage(WarningMessage{ "Invalid response of server" });
			continue;
		}

		if (response->is_404_NotFound())
		{

		}

		if (response->is_301_MovedPermanently() ||
			response->is_302_MovedTemporarily())
		{
			sendMessage(UrlMessage{ settings()->startUrlAddress().host() + url.relativePath(), std::string(), std::string(), std::string(),
				response->responseCode(), CrawlerModel::InternalCrawledUrlQueue });

			handleRedirection(response, request);
		}
		else
		{
			tagParser.parseTags(response->entityBody(), "a");

			//
			// TODO: remove from tagParser all HTTPS urls
			//

			model()->saveUniqueUrls(tagParser, settings()->startUrlAddress(), url);

			//
			// Find meta description
			//

			std::string description;
			std::string charset;

			tagParser.parseTags(response->entityBody(), "meta");

			if (tagParser.size())
			{
				auto findDescription = [](const Tag& tag)
				{
					return Common::strToLower(tag.attribute("name")) == "description";
				};

				auto findCharset = [](const Tag& tag)
				{
					return Common::strToLower(tag.attribute("http-equiv")) == "content-type";
				};

				auto iterDescription = std::find_if(std::begin(tagParser), std::end(tagParser), findDescription);
				auto iterCharset = std::find_if(std::begin(tagParser), std::end(tagParser), findCharset);

				if (iterCharset != std::end(tagParser))
				{
					charset = iterCharset->attribute("content");
				}

				if (iterDescription != std::end(tagParser))
				{
					description = iterDescription->attribute("content");

					if (!description.empty())
					{
						model()->addDescription(url, description);

						if (model()->isDuplicatedDescription(url, description))
						{
							if (model()->duplicatesDescription(url, description) == 2)
							{
								sendMessage(DuplicatedDescriptionMessage{
									settings()->startUrlAddress().host() + model()->firstDuplicatedDescription(description),
									description,
									charset
								});
							}

							sendMessage(DuplicatedDescriptionMessage{ settings()->startUrlAddress().host() + url.relativePath(), description, charset });
						}
					}
					else
					{
						sendMessage(EmptyDescriptionMessage{ settings()->startUrlAddress().host() + url.relativePath() });
					}
				}
			}

			//
			// Find title tag
			//

			tagParser.parseTagsWithValues(response->entityBody(), "title");

			std::string title;

			if (!tagParser.size() || (tagParser.size() && tagParser[0].value().empty()))
			{
				sendMessage(EmptyTitleMessage{ settings()->startUrlAddress().host() + url.relativePath() });
			}
			else
			{
				title = tagParser[0].value();

				model()->addTitle(url, title);

				if (model()->isDuplicatedTitle(url, title))
				{
					if (model()->duplicatesTitle(url, title) == 2)
					{
						sendMessage(DuplicatedTitleMessage{ settings()->startUrlAddress().host() + model()->firstDuplicatedTitle(title), title, charset });
					}

					sendMessage(DuplicatedTitleMessage{ settings()->startUrlAddress().host() + url.relativePath(), title, charset });
				}
			}

			sendMessage(UrlMessage{ settings()->startUrlAddress().host() + url.relativePath(), title, description, charset,
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

		model()->queue(CrawlerModel::InternalCrawledUrlQueue)->clear();

		if (!model()->isItemExistsIn(url, CrawlerModel::InternalCrawledUrlQueue))
		{
			model()->storeUrl(url, CrawlerModel::InternalUrlQueue);
		}
	}
}

}