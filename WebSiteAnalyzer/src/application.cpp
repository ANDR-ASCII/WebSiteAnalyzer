#include "application.h"
#include "message_mapper.h"

namespace WebSiteAnalyzer
{

Application::Application(int argc, char** argv)
	: QApplication(argc, argv)
	, m_mainFrame(new MainFrame)
{
	m_mainFrame->show();
}

}
