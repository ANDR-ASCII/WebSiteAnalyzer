#pragma once

#include "stdlibs.h"
#include "ui_start_settings_dialog.h"

namespace WebSiteAnalyzer
{

class StartSettingsDialog 
	: public QDialog
	, private Ui::StartSettingsDialog
{
	Q_OBJECT

public:
	StartSettingsDialog(QWidget* parent);

	QString startAddress() const;
	std::size_t maxCrawlUrls() const;
	std::size_t requestPause() const;

private:
	Q_SLOT void slot_checkAcceptedData();
};

}
