#include "duplicated_descriptions_model.h"

namespace WebSiteAnalyzer
{

DuplicatedDescriptionsModel::DuplicatedDescriptionsModel(QObject* parent)
	: QAbstractTableModel(parent)
{
	QTextCodec* codec = QTextCodec::codecForLocale();

	m_headerData
		<< "URL"
		<< codec->toUnicode("Метаопсиание");
}

int DuplicatedDescriptionsModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(m_urlDescriptionPairs.size());
}

int DuplicatedDescriptionsModel::columnCount(const QModelIndex& parent) const
{
	return m_headerData.size();
}

QVariant DuplicatedDescriptionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		assert(section < m_headerData.size());

		return m_headerData[section];
	}

	return QVariant();
}

QVariant DuplicatedDescriptionsModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || !rowCount())
	{
		assert(index.row() < static_cast<int>(m_urlDescriptionPairs.size()));

		const std::pair<QString, QString>& pair = m_urlDescriptionPairs[index.row()];

		return QVariant(index.column() == 0 ? pair.first : pair.second);
	}

	return QVariant();
}

bool DuplicatedDescriptionsModel::removeRows(int row, int count, const QModelIndex &parent /*= QModelIndex()*/)
{
	beginResetModel();
	m_urlDescriptionPairs.clear();
	endResetModel();

	return true;
}

void DuplicatedDescriptionsModel::slot_addUrl(const std::string& url, const std::string& description, const std::string& charset)
{
	const bool isUtf8 = charset.find("utf-8") != std::string::npos;
	QTextCodec* codec = QTextCodec::codecForLocale();

	beginResetModel();

	auto pair = std::make_pair(
		isUtf8 ? QString(url.c_str()) : QString(codec->toUnicode(url.c_str())),
		isUtf8 ? QString(description.c_str()) : QString(codec->toUnicode(description.c_str()))
	);

	m_urlDescriptionPairs.push_front(std::move(pair));

	endResetModel();
}

}