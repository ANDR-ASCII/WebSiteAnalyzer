#include "duplicated_titles_model.h"

namespace WebSiteAnalyzer
{

DuplicatedTitlesModel::DuplicatedTitlesModel(QObject* parent)
	: QAbstractTableModel(parent)
{
	QTextCodec* codec = QTextCodec::codecForLocale();

	m_headerData
		<< "URL"
		<< codec->toUnicode("Заголовок страницы");
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

bool DuplicatedTitlesModel::removeRows(int row, int count, const QModelIndex &parent /*= QModelIndex()*/)
{
	beginResetModel();
	m_urlTitlePairs.clear();
	endResetModel();

	return true;
}

void DuplicatedTitlesModel::slot_addUrl(const std::string& url, const std::string& title, const std::string& charset)
{
	const bool isUtf8 = charset.find("utf-8") != std::string::npos;
	QTextCodec* codec = QTextCodec::codecForLocale();

	beginResetModel();

	auto pair = std::make_pair(
		isUtf8 ? QString(url.c_str()) : QString(codec->toUnicode(url.c_str())),
		isUtf8 ? QString(title.c_str()) : QString(codec->toUnicode(title.c_str()))
	);

	m_urlTitlePairs.push_front(std::move(pair));

	endResetModel();
}

}