#include "duplicated_titles_model.h"

namespace WebSiteAnalyzer
{

DuplicatedTitlesModel::DuplicatedTitlesModel(QObject* parent)
	: QAbstractTableModel(parent)
{
	m_headerData
		<< "URL"
		<< "Title";
}

int DuplicatedTitlesModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(m_urlTitlePairs.size());
}

int DuplicatedTitlesModel::columnCount(const QModelIndex& parent) const
{
	return m_headerData.size();
}

QVariant DuplicatedTitlesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		assert(section < m_headerData.size());

		return m_headerData[section];
	}

	return QVariant();
}

QVariant DuplicatedTitlesModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || !rowCount())
	{
		assert(index.row() < static_cast<int>(m_urlTitlePairs.size()));

		const std::pair<QString, QString>& pair = m_urlTitlePairs[index.row()];

		return QVariant(index.column() == 0 ? pair.first : pair.second);
	}

	return QVariant();
}

void DuplicatedTitlesModel::slot_addUrl(const std::string& url, const std::string& title)
{
	beginResetModel();

	m_urlTitlePairs.push_front(std::make_pair(QString(url.c_str()), QString(title.c_str())));

	endResetModel();
}

}