#include "pages_404_model.h"

namespace WebSiteAnalyzer
{

Pages404Model::Pages404Model(QObject* parent)
	: QAbstractTableModel(parent)
{
	m_headerData << "URL";
}

int Pages404Model::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(m_urls.size());
}

int Pages404Model::columnCount(const QModelIndex& parent) const
{
	return m_headerData.size();
}

QVariant Pages404Model::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		assert(section < m_headerData.size());

		return m_headerData[section];
	}

	return QVariant();
}

QVariant Pages404Model::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || !rowCount())
	{
		return m_urls[index.row()];
	}

	return QVariant();
}

bool Pages404Model::removeRows(int row, int count, const QModelIndex &parent /*= QModelIndex()*/)
{
	beginResetModel();
	m_urls.clear();
	endResetModel();

	return true;
}

void Pages404Model::slot_addUrl(const std::string& url)
{
	beginResetModel();

	m_urls.push_front(QString(url.c_str()));

	endResetModel();
}

}