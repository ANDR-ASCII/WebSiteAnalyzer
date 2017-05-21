#include "empty_descriptions_model.h"

namespace WebSiteAnalyzer
{

EmptyDescriptionsModel::EmptyDescriptionsModel(QObject* parent)
	: QAbstractTableModel(parent)
{
	m_headerData << "URL";
}

int EmptyDescriptionsModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(m_urls.size());
}

int EmptyDescriptionsModel::columnCount(const QModelIndex& parent) const
{
	return m_headerData.size();
}

QVariant EmptyDescriptionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		assert(section < m_headerData.size());

		return m_headerData[section];
	}

	return QVariant();
}

QVariant EmptyDescriptionsModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || !rowCount())
	{
		return m_urls[index.row()];
	}

	return QVariant();
}

void EmptyDescriptionsModel::slot_addUrl(const std::string& url)
{
	beginResetModel();

	m_urls.push_front(QString(url.c_str()));

	endResetModel();
}

}