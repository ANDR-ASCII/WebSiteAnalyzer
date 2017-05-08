#include "main_frame.h"
#include "start_settings_dialog.h"

namespace WebSiteAnalyzer
{

MainFrame::MainFrame(QWidget *parent)
	: QMainWindow(parent)
	, m_startSettingsDialog(new StartSettingsDialog(this))
{
	ui.setupUi(this);

	connect(ui.startCrawlerButton, SIGNAL(clicked()), SLOT(slot_showStartSettingsDialog()));

	m_crawlerController.setModel(&m_crawlerModel);
	m_crawlerController.setSettings(&m_crawlerSettings);
}

void MainFrame::slot_showStartSettingsDialog()
{
	if (m_startSettingsDialog->exec() == QDialog::Accepted)
	{
		m_crawlerSettings.setHost(m_startSettingsDialog->startAddress().toStdString());
		m_crawlerSettings.setMaxCrawlUrls(m_startSettingsDialog->maxCrawlUrls());
		m_crawlerSettings.setRequestPause(std::chrono::milliseconds(m_startSettingsDialog->requestPause()));

		m_crawlerController.startCrawling();
	}
}

void MainFrame::receiveMessage(const CrawlerImpl::IMessage& message)
{
	switch (message.type())
	{
		case CrawlerImpl::IMessage::MessageType::WarningType:
		{
			const CrawlerImpl::WarningMessage& actualMessage =
				static_cast<const CrawlerImpl::WarningMessage&>(message);

			ui.logInfo->append(QString(actualMessage.warning().c_str()));

			break;
		}
	}
}

}