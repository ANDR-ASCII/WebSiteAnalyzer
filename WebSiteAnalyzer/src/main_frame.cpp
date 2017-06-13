#include "main_frame.h"
#include "start_settings_dialog.h"
#include "crawler_settings.h"
#include "external_urls_model.h"
#include "urls_crawler_model.h"
#include "pages_404_model.h"
#include "duplicated_titles_model.h"
#include "duplicated_descriptions_model.h"
#include "empty_descriptions_model.h"
#include "empty_titles_model.h"



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
	QTextCodec* codec = QTextCodec::codecForLocale();

	if (m_worker->isStopped())
	{
		QMessageBox continueDialog;

		continueDialog.setWindowTitle(codec->toUnicode("Уведомление"));
		continueDialog.setText(codec->toUnicode("Продолжить?"));
		continueDialog.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

		if (continueDialog.exec() == QMessageBox::Ok)
		{
			emit signal_startCrawlerCommand(m_settings.get(), true);

			ui.progressBar->show();
		}
	}
	else
	{
		if (m_startSettingsDialog->exec() == QDialog::Accepted)
		{
			m_settings->setHost(m_startSettingsDialog->startAddress().toStdString());
			m_settings->setMaxCrawlUrls(m_startSettingsDialog->maxCrawlUrls());
			m_settings->setRequestPause(std::chrono::milliseconds(m_startSettingsDialog->requestPause()));

			clearModels();

			emit signal_startCrawlerCommand(m_settings.get(), false);

			ui.progressBar->show();
		}
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

void MainFrame::clearModels()
{
	ui.crawlerTableView->model()->removeRows(0, ui.crawlerTableView->model()->rowCount(), QModelIndex());
	ui.externalUrlsTableView->model()->removeRows(0, ui.externalUrlsTableView->model()->rowCount(), QModelIndex());
	ui.pages404TableView->model()->removeRows(0, ui.pages404TableView->model()->rowCount(), QModelIndex());
	ui.titleDuplicatesTableView->model()->removeRows(0, ui.titleDuplicatesTableView->model()->rowCount(), QModelIndex());
	ui.descriptionDuplicatesTableView->model()->removeRows(0, ui.descriptionDuplicatesTableView->model()->rowCount(), QModelIndex());
	ui.missingTitleTableView->model()->removeRows(0, ui.missingTitleTableView->model()->rowCount(), QModelIndex());
	ui.missingDescriptionTableView->model()->removeRows(0, ui.missingDescriptionTableView->model()->rowCount(), QModelIndex());
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
	EmptyDescriptionsModel* emptyDescriptionsModel = new EmptyDescriptionsModel(ui.missingDescriptionTableView);
	EmptyTitlesModel* emptyTitlesModel = new EmptyTitlesModel(ui.missingTitleTableView);

	ui.progressBar->setValue(0);
	ui.progressBar->hide();


	ui.crawlerTableView->verticalHeader()->hide();
	ui.crawlerTableView->horizontalHeader()->setDefaultSectionSize(width() / crawlerModel->columnCount());
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

	ui.missingDescriptionTableView->verticalHeader()->hide();
	ui.missingDescriptionTableView->horizontalHeader()->setDefaultSectionSize(600);
	ui.missingDescriptionTableView->setModel(emptyDescriptionsModel);

	ui.missingTitleTableView->verticalHeader()->hide();
	ui.missingTitleTableView->horizontalHeader()->setDefaultSectionSize(600);
	ui.missingTitleTableView->setModel(emptyTitlesModel);


	m_worker = new CrawlerWorker;
	m_worker->moveToThread(&m_crawlerThread);


	VERIFY(connect(&m_crawlerThread, &QThread::finished,
		m_worker, &QObject::deleteLater));

	//
	// model/view connections
	//

	VERIFY(connect(m_worker, &CrawlerWorker::signal_addUrl,
		crawlerModel, &UrlsCrawlerModel::slot_addUrl, Qt::BlockingQueuedConnection));

	VERIFY(connect(m_worker, &CrawlerWorker::signal_addExternalUrl,
		externalUrlsModel, &ExternalUrlsModel::slot_addUrl, Qt::BlockingQueuedConnection));

	VERIFY(connect(m_worker, &CrawlerWorker::signal_add404Url,
		pages404Model, &Pages404Model::slot_addUrl, Qt::BlockingQueuedConnection));

	VERIFY(connect(m_worker, &CrawlerWorker::signal_addDuplicatedTitleUrl,
		duplicatedTitlesModel, &DuplicatedTitlesModel::slot_addUrl, Qt::BlockingQueuedConnection));

	VERIFY(connect(m_worker, &CrawlerWorker::signal_addDuplicatedDescriptionUrl,
		duplicatedDescriptionsModel, &DuplicatedDescriptionsModel::slot_addUrl, Qt::BlockingQueuedConnection));

	VERIFY(connect(m_worker, &CrawlerWorker::signal_addEmptyDescriptionUrl,
		emptyDescriptionsModel, &EmptyDescriptionsModel::slot_addUrl, Qt::BlockingQueuedConnection));

	VERIFY(connect(m_worker, &CrawlerWorker::signal_addEmptyTitleUrl,
		emptyTitlesModel, &EmptyTitlesModel::slot_addUrl, Qt::BlockingQueuedConnection));



	VERIFY(connect(m_worker, &CrawlerWorker::signal_queueSize,
		this, &MainFrame::slot_queueSize, Qt::BlockingQueuedConnection));

	VERIFY(connect(m_worker, &CrawlerWorker::signal_DNSError,
		this, &MainFrame::slot_DNSError, Qt::QueuedConnection));
	
	VERIFY(connect(m_worker, &CrawlerWorker::signal_progressStopped,
		ui.progressBar, &QProgressBar::hide, Qt::QueuedConnection));

	//
	// slot_stopCrawler must execute only atomic operations!
	//

	VERIFY(connect(this, &MainFrame::signal_stopCrawlerCommand,
		m_worker, &CrawlerWorker::slot_stopCrawler, Qt::DirectConnection));

	VERIFY(connect(this, &MainFrame::signal_startCrawlerCommand,
		m_worker, &CrawlerWorker::slot_startCrawler));
}

}