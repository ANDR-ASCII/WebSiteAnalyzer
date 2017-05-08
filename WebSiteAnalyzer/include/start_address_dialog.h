#pragma once

#include "stdlibs.h"
#include "ui_start_address_dialog.h"

namespace WebSiteAnalyzer
{

class StartAddressDialog 
	: public QDialog
	, private Ui::StartAddressDialog
{
	Q_OBJECT

public:
	StartAddressDialog();

	const QString& startAddress() const;
};

}
