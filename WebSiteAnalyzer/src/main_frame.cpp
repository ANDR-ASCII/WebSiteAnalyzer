#include "main_frame.h"
#include "start_address_dialog.h"

namespace WebSiteAnalyzer
{

MainFrame::MainFrame(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.startCrawlerButton, SIGNAL(clicked()), &m_startAddressDialog, SLOT(exec()));
}

MainFrame::~MainFrame()
{

}

}