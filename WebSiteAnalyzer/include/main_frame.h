#pragma once

#include "stdlibs.h"
#include "ui_main_frame.h"
#include "start_settings_dialog.h"
#include "crawler_settings.h"
#include "crawler_model.h"
#include "crawler_controller.h"

namespace WebSiteAnalyzer
{

class MainFrame 
	: public QMainWindow
	, public CrawlerImpl::IMessageReceiver
{
	Q_OBJECT

public:
	MainFrame(QWidget *parent = 0);

protected:
	virtual void receiveMessage(const CrawlerImpl::IMessage& message) override;

private:
	Q_SLOT void slot_showStartSettingsDialog();

private:
	Ui::MainFrameClass ui;

	std::unique_ptr<StartSettingsDialog> m_startSettingsDialog;

	CrawlerImpl::CrawlerSettings m_crawlerSettings;
	CrawlerImpl::CrawlerModel m_crawlerModel;
	CrawlerImpl::CrawlerController m_crawlerController;
};

}