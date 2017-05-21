#include "urls_crawler_model.h"

namespace WebSiteAnalyzer
{

UrlsCrawlerModel::UrlsCrawlerModel(QObject* parent)
	: QAbstractTableModel(parent)
{
	QTextCodec* codec = QTextCodec::codecForLocale();

	m_headerData 
		<< "URL" 
		<< codec->toUnicode("Заголовок страницы")
		<< codec->toUnicode("Метаописание страницы")
		<< codec->toUnicode("Код ответа сервера");
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

		return QVariant(m_urls[index.row()][index.column()]);
	}

	return QVariant();
}

void UrlsCrawlerModel::slot_addUrl(const std::string& url, 
	const std::string& title, 
	const std::string& description, 
	const std::string& charset, 
	int responseCode)
{
	const bool isUtf8 = charset.find("utf-8") != std::string::npos;

	QTextCodec* codec = QTextCodec::codecForLocale();

	beginResetModel();

	std::array<QString, 4> temporaryRow
	{
		url.c_str(),
		isUtf8 ? title.c_str() : codec->toUnicode(title.c_str()),
		isUtf8 ? description.c_str() : codec->toUnicode(description.c_str()),
		QString().setNum(responseCode)
	};

	m_urls.push_front(std::move(temporaryRow));

	endResetModel();
}

}