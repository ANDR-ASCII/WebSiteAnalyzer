#pragma once

#include "stdlibs.h"
#include "crawler_model.h"

namespace WebSiteAnalyzer
{

class UrlsCrawlerModel : public QAbstractListModel
{
	Q_OBJECT
public:
	UrlsCrawlerModel(QObject* parent = nullptr);
	
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	Q_SLOT void slot_addUrl(const std::string& url);

private:
	std::deque<QString> m_urls;
};

}
