#include "external_urls_model.h"

namespace WebSiteAnalyzer
{

ExternalUrlsModel::ExternalUrlsModel(QObject* parent)
	: QAbstractTableModel(parent)
{
	m_headerData << "URL";
}

int ExternalUrlsModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(m_urls.size());
}

int ExternalUrlsModel::columnCount(const QModelIndex& parent) const
{
	return m_headerData.size();
}

QVariant ExternalUrlsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		assert(section < m_headerData.size());

		return m_headerData[section];
	}

	return QVariant();
}

QVariant ExternalUrlsModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || !rowCount())
	{
		return m_urls[index.row()];
	}

	return QVariant();
}

bool ExternalUrlsModel::removeRows(int row, int count, const QModelIndex &parent)
{
	beginResetModel();
	m_urls.clear();
	endResetModel();

	return true;
}

void ExternalUrlsModel::slot_addUrl(const std::string& url)
{
	beginResetModel();

	m_urls.push_front(QString(url.c_str()));

	endResetModel();
}

}