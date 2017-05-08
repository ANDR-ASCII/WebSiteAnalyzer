#pragma once

#include "stdlibs.h"
#include "ui_main_frame.h"
#include "start_address_dialog.h"

namespace WebSiteAnalyzer
{

class MainFrame 
	: public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget *parent = 0);
	~MainFrame();

private:
	Ui::MainFrameClass ui;
	StartAddressDialog m_startAddressDialog;
};

}