#include "application.h"
#include "message_mapper.h"

namespace WebSiteAnalyzer
{

Application::Application(int argc, char** argv)
	: m_app(argc, argv)
	, m_model(new CrawlerImpl::CrawlerModel)
	, m_controller(new CrawlerImpl::CrawlerController)
	, m_stopCrawler(false)
	, m_mainFrame(new MainFrame(m_model.get()))
{
	m_controller->setModel(m_model.get());

	connect(m_mainFrame.get(), SIGNAL(signal_startCrawling(CrawlerImpl::CrawlerSettings*)), 
		SLOT(slot_startCrawlingCommand(CrawlerImpl::CrawlerSettings*)));

	m_mainFrame->show();
}

void Application::crawlerThread()
{
	MessageMapper mapper;

	connect(&mapper, SIGNAL(signal_modelUpdated()), 
		m_mainFrame.get(), SLOT(slot_modelUpdated()), Qt::BlockingQueuedConnection
	);

	m_controller->addReceiver(&mapper);
	m_controller->startCrawling(m_stopCrawler);
}

int Application::exec()
{
	return m_app.exec();
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
