#include "main_frame.h"
#include "start_settings_dialog.h"

namespace WebSiteAnalyzer
{

MainFrame::MainFrame(QWidget *parent)
	: QMainWindow(parent)
	, m_startSettingsDialog(new StartSettingsDialog(this))
	, m_crawlerModel(new UrlsCrawlerModel)
	, m_settings(new CrawlerSettings)
	, m_model(new CrawlerModel)
	, m_controller(new CrawlerController)
	, m_needToStopCrawler(false)
	, m_crawlerActuallyStopped(true)
{
	ui.setupUi(this);

	m_crawlerModel->setParent(ui.crawlerListView);
	m_crawlerModel->setInternalModel(m_model.get());

	ui.crawlerListView->setModel(m_crawlerModel.get());

	VERIFY(connect(ui.startCrawlerButton, SIGNAL(clicked()), SLOT(slot_showStartSettingsDialog())));
	VERIFY(connect(ui.stopCrawlerButton, SIGNAL(clicked()), SIGNAL(signal_stopCrawlerCommand())));
}

void MainFrame::slot_showStartSettingsDialog()
{
	if (m_startSettingsDialog->exec() == QDialog::Accepted)
	{
		m_settings->setHost(m_startSettingsDialog->startAddress().toStdString());
		m_settings->setMaxCrawlUrls(m_startSettingsDialog->maxCrawlUrls());
		m_settings->setRequestPause(std::chrono::milliseconds(m_startSettingsDialog->requestPause()));

		emit signal_startCrawling(m_settings.get());
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