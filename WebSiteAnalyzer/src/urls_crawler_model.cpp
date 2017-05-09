#include "urls_crawler_model.h"

namespace WebSiteAnalyzer
{

UrlsCrawlerModel::UrlsCrawlerModel(QObject* parent)
	: QAbstractListModel(parent)
	, m_internalModel(nullptr)
	, m_oldSize(0)
{
}

int UrlsCrawlerModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(m_internalModel->size(CrawlerImpl::CrawlerModel::InternalCrawledUrlQueue));
}

QVariant UrlsCrawlerModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || !rowCount())
	{
		return QVariant(QString((*m_internalModel->queue(CrawlerImpl::CrawlerModel::InternalCrawledUrlQueue))[index.row()].c_str()));
	}

	return QVariant();
}

void UrlsCrawlerModel::setInternalModel(CrawlerImpl::CrawlerModel* internalModel)
{
	m_internalModel = internalModel;
}

void UrlsCrawlerModel::needToUpdate()
{
	QModelIndex topLeft = createIndex(m_oldSize, 1);
	QModelIndex bottomRight = createIndex(m_internalModel->size(CrawlerImpl::CrawlerModel::InternalCrawledUrlQueue), 1);

	m_oldSize = m_internalModel->size(CrawlerImpl::CrawlerModel::InternalCrawledUrlQueue);

	emit dataChanged(topLeft, bottomRight);
}

}