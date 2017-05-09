#include "application.h"
#include "message_mapper.h"

namespace WebSiteAnalyzer
{

Application::Application(int argc, char** argv)
	: m_app(argc, argv)
	, m_model(new CrawlerImpl::CrawlerModel)
	, m_controller(new CrawlerImpl::CrawlerController)
	, m_needToStopCrawler(false)
	, m_crawlerActuallyStopped(true)
	, m_mainFrame(new MainFrame(m_model.get()))
{
	m_controller->setModel(m_model.get());

	VERIFY(connect(m_mainFrame.get(), SIGNAL(signal_startCrawling(CrawlerImpl::CrawlerSettings*)),
		SLOT(slot_startCrawlingCommand(CrawlerImpl::CrawlerSettings*))));

	VERIFY(connect(m_mainFrame.get(), SIGNAL(signal_stopCrawlerCommand()),
		SLOT(slot_stopCrawlerCommand())));

	m_mainFrame->show();
}

void Application::crawlerThread()
{
	m_crawlerActuallyStopped.store(false);

	MessageMapper mapper;

	VERIFY(connect(&mapper, SIGNAL(signal_modelUpdated()), m_mainFrame.get(), SLOT(slot_modelUpdated()), Qt::BlockingQueuedConnection));

	VERIFY(connect(&mapper, SIGNAL(signal_warningMessage(const std::string&)), m_mainFrame.get(),
		SLOT(slot_warningMessage(const std::string&)), Qt::BlockingQueuedConnection));

	m_controller->addReceiver(&mapper);
	m_controller->startCrawling(m_needToStopCrawler);
	m_controller->deleteReceiver(&mapper);

	m_needToStopCrawler.store(false);
	m_crawlerActuallyStopped.store(true);

	m_crawlerStopped.notify_one();
}

int Application::exec()
{
	return m_app.exec();
}

void Application::slot_stopCrawlerCommand()
{
	std::unique_lock<std::mutex> locker(m_mutex);
	m_crawlerStopped.wait(locker, [this] { return m_crawlerActuallyStopped.load(); });
}

void Application::slot_startCrawlingCommand(CrawlerImpl::CrawlerSettings* settings)
{
	m_model->queue(CrawlerImpl::CrawlerModel::InternalUrlQueue)->clear();
	m_model->queue(CrawlerImpl::CrawlerModel::InternalCrawledUrlQueue)->clear();
	m_model->queue(CrawlerImpl::CrawlerModel::ExternalUrlQueue)->clear();

	m_controller->setSettings(settings);

	std::thread crawlingThread = std::thread(&Application::crawlerThread, this);
	crawlingThread.detach();
}

}
