#include "urls_crawler_model.h"

namespace WebSiteAnalyzer
{

UrlsCrawlerModel::UrlsCrawlerModel(QObject* parent)
	: QAbstractTableModel(parent)
{
	m_headerData 
		<< "URL" 
		<< "HTTP Response Code";
}

int UrlsCrawlerModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(m_urls.size());
}

int UrlsCrawlerModel::columnCount(const QModelIndex& parent) const
{
	return m_headerData.size();
}

QVariant UrlsCrawlerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		assert(section < m_headerData.size());

		return m_headerData[section];
	}

	return QVariant();
}

QVariant UrlsCrawlerModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || !rowCount())
	{
		assert(index.row() < static_cast<int>(m_urls.size()));

		const std::pair<QString, int>& pair = m_urls[index.row()];

		return QVariant(index.column() == 0 ? pair.first : QString().setNum(pair.second));
	}

	return QVariant();
}

void UrlsCrawlerModel::slot_addUrl(const std::string& url, int responseCode)
{
	beginResetModel();

	m_urls.push_front(std::make_pair(QString(url.c_str()), responseCode));

	endResetModel();
}

}