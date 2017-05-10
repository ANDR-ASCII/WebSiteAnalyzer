#pragma once

#include "stdlibs.h"
#include "main_frame.h"

namespace WebSiteAnalyzer
{

class Application : public QApplication
{
	Q_OBJECT

public:
	Application(int argc, char** argv);

private:
	std::unique_ptr<MainFrame> m_mainFrame;
};

}
