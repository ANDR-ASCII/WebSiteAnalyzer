#include "urls_crawler_model.h"

namespace WebSiteAnalyzer
{

UrlsCrawlerModel::UrlsCrawlerModel(QObject* parent)
	: QAbstractListModel(parent)
{
}

int UrlsCrawlerModel::rowCount(const QModelIndex& parent) const
{
	return m_internalModel.size(CrawlerImpl::CrawlerModel::InternalCrawledUrlQueue);
}

QVariant UrlsCrawlerModel::data(const QModelIndex& index, int role) const
{
	return QVariant();

	if (role == Qt::DisplayRole)
	{
		//return QString(m_internalModel.anyUrl());
	}
}

}