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

using namespace CrawlerImpl;

class MainFrame 
	: public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget *parent = 0);

	Q_SIGNAL void signal_stopCrawlerCommand();
	Q_SIGNAL void signal_startCrawlerCommand(CrawlerImpl::CrawlerSettings*);

	Q_SLOT void slot_hideProgressBarWhenStoppingCrawler();
	Q_SLOT void slot_showStartSettingsDialog();
	Q_SLOT void slot_queueSize(std::size_t size, int queueType);
	Q_SLOT void slot_DNSError();

private:
	void initialize();
	void initializeModelsAndViews();

private:
	Ui::MainFrameClass ui;
	std::unique_ptr<StartSettingsDialog> m_startSettingsDialog;
	std::unique_ptr<CrawlerSettings> m_settings;

	QThread m_crawlerThread;
};

}