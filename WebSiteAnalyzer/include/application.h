#pragma once

#include "main_frame.h"
#include <memory>
#include <QtWidgets/QApplication>

namespace WebSiteIndexer
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
