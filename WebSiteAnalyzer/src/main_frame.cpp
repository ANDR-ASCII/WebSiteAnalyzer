#include "main_frame.h"
#include "start_settings_dialog.h"

namespace WebSiteAnalyzer
{

MainFrame::MainFrame(CrawlerImpl::CrawlerModel* model, QWidget *parent)
	: QMainWindow(parent)
	, m_startSettingsDialog(new StartSettingsDialog(this))
{
	ui.setupUi(this);

	m_crawlerModel.reset(new UrlsCrawlerModel(ui.crawlerListView));
	m_crawlerModel->setInternalModel(model);

	ui.crawlerListView->setModel(m_crawlerModel.get());

	connect(ui.startCrawlerButton, SIGNAL(clicked()), SLOT(slot_showStartSettingsDialog()));
}

void MainFrame::slot_showStartSettingsDialog()
{
	if (m_startSettingsDialog->exec() == QDialog::Accepted)
	{
		m_crawlerSettings.setHost(m_startSettingsDialog->startAddress().toStdString());
		m_crawlerSettings.setMaxCrawlUrls(m_startSettingsDialog->maxCrawlUrls());
		m_crawlerSettings.setRequestPause(std::chrono::milliseconds(m_startSettingsDialog->requestPause()));

		emit signal_startCrawling(&m_crawlerSettings);
	}
}

void MainFrame::slot_modelUpdated()
{
	m_crawlerModel->needToUpdate();
}

void MainFrame::slot_warningMessage(const std::string& text)
{
	ui.logInfo->append(QString(text.c_str()));
}

}