#include "main_frame.h"
#include "start_settings_dialog.h"
#include "crawler_worker.h"
#include "crawler_settings.h"
#include "external_urls_model.h"
#include "urls_crawler_model.h"
#include "pages_404_model.h"
#include "duplicated_titles_model.h"
#include "duplicated_descriptions_model.h"

namespace WebSiteAnalyzer
{

MainFrame::MainFrame(QWidget* parent)
	: QMainWindow(parent)
	, m_startSettingsDialog(new StartSettingsDialog(this))
	, m_settings(new CrawlerSettings)
{
	initialize();


}

void MainFrame::slot_hideProgressBarWhenStoppingCrawler()
{
	ui.progressBar->hide();
}

void MainFrame::slot_showStartSettingsDialog()
{
	if (m_startSettingsDialog->exec() == QDialog::Accepted)
	{
		m_settings->setHost(m_startSettingsDialog->startAddress().toStdString());
		m_settings->setMaxCrawlUrls(m_startSettingsDialog->maxCrawlUrls());
		m_settings->setRequestPause(std::chrono::milliseconds(m_startSettingsDialog->requestPause()));

		emit signal_startCrawlerCommand(m_settings.get());

		ui.progressBar->show();
	}
}

void MainFrame::slot_queueSize(std::size_t size, int queueType)
{
	if (queueType == (CrawlerModel::InternalUrlQueue | CrawlerModel::InternalCrawledUrlQueue))
	{
		ui.amountLabel->setText(QString().setNum(static_cast<int>(size)));
	}

	if (queueType == CrawlerModel::InternalCrawledUrlQueue)
	{
		ui.crawledCountLabel->setText(QString().setNum(static_cast<int>(size)));
	}

	if (queueType == CrawlerModel::ExternalUrlQueue)
	{
		ui.externalUrlsCountLabel->setText(QString().setNum(static_cast<int>(size)));
	}

	double amount = ui.amountLabel->text().toDouble() + 1;
	double crawled = ui.crawledCountLabel->text().toDouble() + 1;

	double percent = amount / 100;

	ui.progressBar->setValue(crawled / (percent ? percent : percent + 1));
}

void MainFrame::slot_DNSError()
{
	QMessageBox::critical(this, "DNS Error", "Cannot resolve address", QMessageBox::Ok);
}

void MainFrame::initialize()
{
	ui.setupUi(this);
	setWindowIcon(QIcon(QStringLiteral("images/spider_icon.png")));

	VERIFY(connect(ui.startCrawlerButton, &QPushButton::clicked, this, &MainFrame::slot_showStartSettingsDialog));
	VERIFY(connect(ui.stopCrawlerButton, &QPushButton::clicked, this, &MainFrame::signal_stopCrawlerCommand));
	VERIFY(connect(ui.stopCrawlerButton, &QPushButton::clicked, this, &MainFrame::slot_hideProgressBarWhenStoppingCrawler));

	initializeModelsAndViews();

	m_crawlerThread.start();
}

void MainFrame::initializeModelsAndViews()
{
	UrlsCrawlerModel* crawlerModel = new UrlsCrawlerModel(ui.crawlerTableView);
	ExternalUrlsModel* externalUrlsModel = new ExternalUrlsModel(ui.crawlerTableView);
	Pages404Model* pages404Model = new Pages404Model(ui.pages404TableView);
	DuplicatedTitlesModel* duplicatedTitlesModel = new DuplicatedTitlesModel(ui.titleDuplicatesTableView);
	DuplicatedDescriptionsModel* duplicatedDescriptionsModel = new DuplicatedDescriptionsModel(ui.descriptionDuplicatesTableView);

	ui.progressBar->setValue(0);
	ui.progressBar->hide();


	ui.crawlerTableView->verticalHeader()->hide();
	ui.crawlerTableView->horizontalHeader()->setDefaultSectionSize(ui.crawlerTableView->width() / crawlerModel->columnCount());
	ui.crawlerTableView->setModel(crawlerModel);


	ui.externalUrlsTableView->verticalHeader()->hide();
	ui.externalUrlsTableView->horizontalHeader()->setDefaultSectionSize(600);
	ui.externalUrlsTableView->setModel(externalUrlsModel);


	ui.pages404TableView->verticalHeader()->hide();
	ui.pages404TableView->horizontalHeader()->setDefaultSectionSize(600);
	ui.pages404TableView->setModel(pages404Model);


	ui.titleDuplicatesTableView->verticalHeader()->hide();
	ui.titleDuplicatesTableView->horizontalHeader()->setDefaultSectionSize(600);
	ui.titleDuplicatesTableView->setModel(duplicatedTitlesModel);


	ui.descriptionDuplicatesTableView->verticalHeader()->hide();
	ui.descriptionDuplicatesTableView->horizontalHeader()->setDefaultSectionSize(600);
	ui.descriptionDuplicatesTableView->setModel(duplicatedDescriptionsModel);


	CrawlerWorker* worker = new CrawlerWorker;
	worker->moveToThread(&m_crawlerThread);


	VERIFY(connect(&m_crawlerThread, &QThread::finished,
		worker, &QObject::deleteLater));

	//
	// model/view connections
	//

	VERIFY(connect(worker, &CrawlerWorker::signal_addUrl,
		crawlerModel, &UrlsCrawlerModel::slot_addUrl, Qt::BlockingQueuedConnection));

	VERIFY(connect(worker, &CrawlerWorker::signal_addExternalUrl,
		externalUrlsModel, &ExternalUrlsModel::slot_addUrl, Qt::BlockingQueuedConnection));

	VERIFY(connect(worker, &CrawlerWorker::signal_add404Url,
		pages404Model, &Pages404Model::slot_addUrl, Qt::BlockingQueuedConnection));

	VERIFY(connect(worker, &CrawlerWorker::signal_addDuplicatedTitleUrl,
		duplicatedTitlesModel, &DuplicatedTitlesModel::slot_addUrl, Qt::BlockingQueuedConnection));

	VERIFY(connect(worker, &CrawlerWorker::signal_addDuplicatedDescriptionUrl,
		duplicatedDescriptionsModel, &DuplicatedDescriptionsModel::slot_addUrl, Qt::BlockingQueuedConnection));



	VERIFY(connect(worker, &CrawlerWorker::signal_queueSize,
		this, &MainFrame::slot_queueSize, Qt::BlockingQueuedConnection));

	VERIFY(connect(worker, &CrawlerWorker::signal_DNSError, 
		this, &MainFrame::slot_DNSError, Qt::QueuedConnection));


	//
	// slot_stopCrawler must execute only atomic operations!
	//

	VERIFY(connect(this, &MainFrame::signal_stopCrawlerCommand,
		worker, &CrawlerWorker::slot_stopCrawler, Qt::DirectConnection));

	VERIFY(connect(this, &MainFrame::signal_startCrawlerCommand,
		worker, &CrawlerWorker::slot_startCrawler));
}

}