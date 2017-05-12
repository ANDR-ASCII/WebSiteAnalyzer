#include "urls_crawler_model.h"

namespace WebSiteAnalyzer
{

UrlsCrawlerModel::UrlsCrawlerModel(QObject* parent)
	: QAbstractListModel(parent)
{
}

int UrlsCrawlerModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(m_urls.size());
}

QVariant UrlsCrawlerModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || !rowCount())
	{
		return QVariant(m_urls[index.row()]);
	}

	return QVariant();
}

void UrlsCrawlerModel::slot_addUrl(const std::string& url, int responseCode)
{
	QModelIndex topLeft = createIndex(static_cast<int>(m_urls.size()), 0, this);

	m_urls.push_front(QString(url.c_str()));

	QModelIndex bottomRight = createIndex(static_cast<int>(m_urls.size()), 0, this);

	emit dataChanged(topLeft, bottomRight);
}

}