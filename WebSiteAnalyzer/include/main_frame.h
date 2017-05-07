#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_main_frame.h"

class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget *parent = 0);
	~MainFrame();

private:
	Ui::MainFrameClass ui;
};