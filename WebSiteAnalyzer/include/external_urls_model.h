#pragma once

#include "stdlibs.h"
#include "crawler_model.h"

namespace WebSiteAnalyzer
{

class ExternalUrlsModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	ExternalUrlsModel(QObject* parent = nullptr);

	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

	Q_SLOT void slot_addUrl(const std::string& url);

private:
	std::deque<QString> m_urls;
	QStringList m_headerData;
};

}