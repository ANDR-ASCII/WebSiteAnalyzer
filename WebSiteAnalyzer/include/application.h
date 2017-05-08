#pragma once

#include "stdlibs.h"
#include "main_frame.h"

namespace WebSiteAnalyzer
{

class Application
{
public:
	Application(int argc, char** argv);

	int exec();

private:
	QApplication m_app;
	std::unique_ptr<MainFrame> m_mainFrame;
};

}
