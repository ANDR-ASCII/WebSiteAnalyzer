#pragma once

#include "stdlibs.h"
#include "ui_main_frame.h"
#include "start_settings_dialog.h"
#include "urls_crawler_model.h"
#include "crawler_settings.h"
#include "crawler_model.h"
#include "crawler_controller.h"

namespace WebSiteAnalyzer
{

class MainFrame 
	: public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(CrawlerImpl::CrawlerModel* model, QWidget *parent = 0);

	Q_SIGNAL void signal_startCrawling(CrawlerImpl::CrawlerSettings* settings);
	Q_SIGNAL void signal_stopCrawlerCommand();
	Q_SLOT void slot_showStartSettingsDialog();
	Q_SLOT void slot_modelUpdated();
	Q_SLOT void slot_warningMessage(const std::string&);

private:
	Ui::MainFrameClass ui;
	std::unique_ptr<StartSettingsDialog> m_startSettingsDialog;
	CrawlerImpl::CrawlerSettings m_crawlerSettings;

	std::unique_ptr<UrlsCrawlerModel> m_crawlerModel;
};

}