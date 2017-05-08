#include "application.h"

namespace WebSiteAnalyzer
{

Application::Application(int argc, char** argv)
	: m_app(argc, argv)
	, m_mainFrame(new MainFrame)
{
	m_mainFrame->show();
}

int Application::exec()
{
	return m_app.exec();
}

}
