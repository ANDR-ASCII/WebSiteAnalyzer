#include "main_frame.h"
#include "start_settings_dialog.h"
#include "crawler_worker.h"
#include "crawler_settings.h"

namespace WebSiteAnalyzer
{

MainFrame::MainFrame(QWidget *parent)
	: QMainWindow(parent)
	, m_startSettingsDialog(new StartSettingsDialog(this))
	, m_crawlerModel(new UrlsCrawlerModel)
	, m_settings(new CrawlerSettings)
{
	ui.setupUi(this);

	m_crawlerModel->setParent(ui.crawlerListView);
	ui.crawlerListView->setModel(m_crawlerModel.get());

	VERIFY(connect(ui.startCrawlerButton, SIGNAL(clicked()), SLOT(slot_showStartSettingsDialog())));
	VERIFY(connect(ui.stopCrawlerButton, SIGNAL(clicked()), SIGNAL(signal_stopCrawlerCommand())));

	CrawlerWorker* worker = new CrawlerWorker;
	worker->moveToThread(&m_crawlerThread);

	VERIFY(connect(&m_crawlerThread, &QThread::finished, 
		worker, &QObject::deleteLater));
	
	VERIFY(connect(worker, &CrawlerWorker::signal_addUrl,
		m_crawlerModel.get(), &UrlsCrawlerModel::slot_addUrl, Qt::BlockingQueuedConnection));

	VERIFY(connect(worker, &CrawlerWorker::signal_queueSize,
		this, &MainFrame::slot_queueSize, Qt::BlockingQueuedConnection));

	VERIFY(connect(this, &MainFrame::signal_startCrawlerCommand, 
		worker, &CrawlerWorker::slot_startCrawler));

	m_crawlerThread.start();
}

void MainFrame::slot_showStartSettingsDialog()
{
	if (m_startSettingsDialog->exec() == QDialog::Accepted)
	{
		m_settings->setHost(m_startSettingsDialog->startAddress().toStdString());
		m_settings->setMaxCrawlUrls(m_startSettingsDialog->maxCrawlUrls());
		m_settings->setRequestPause(std::chrono::milliseconds(m_startSettingsDialog->requestPause()));

		emit signal_startCrawlerCommand(m_settings.get());
	}
}

void MainFrame::slot_queueSize(std::size_t size, int queueType)
{
	if (queueType == CrawlerModel::InternalUrlQueue)
	{
		ui.remainderLabel->setText(QString().setNum(static_cast<int>(size)));
	}

	if (queueType == CrawlerModel::InternalCrawledUrlQueue)
	{
		ui.crawledCountLabel->setText(QString().setNum(static_cast<int>(size)));
	}

	if (queueType == CrawlerModel::ExternalUrlQueue)
	{
		ui.externalUrlsCountLabel->setText(QString().setNum(static_cast<int>(size)));
	}

	int first = ui.remainderLabel->text().toInt() + 1;
	int second = ui.crawledCountLabel->text().toInt() + 1;

	int result = first / second;

	ui.progressBar->setValue(result);
}

}