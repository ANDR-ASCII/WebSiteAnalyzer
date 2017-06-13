#pragma once

#include "stdlibs.h"
#include "ui_main_frame.h"
#include "start_settings_dialog.h"
#include "crawler_settings.h"
#include "crawler_worker.h"

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
	Q_SIGNAL void signal_startCrawlerCommand(CrawlerImpl::CrawlerSettings*, bool);

	Q_SLOT void slot_hideProgressBarWhenStoppingCrawler();
	Q_SLOT void slot_showStartSettingsDialog();
	Q_SLOT void slot_queueSize(std::size_t size, int queueType);
	Q_SLOT void slot_DNSError();

private:
	void clearModels();
	void initialize();
	void initializeModelsAndViews();

private:
	Ui::MainFrameClass ui;
	std::unique_ptr<StartSettingsDialog> m_startSettingsDialog;
	std::unique_ptr<CrawlerSettings> m_settings;

	CrawlerWorker* m_worker;
	QThread m_crawlerThread;
};

}