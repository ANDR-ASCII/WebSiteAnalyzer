#include "start_address_dialog.h"

namespace WebSiteAnalyzer
{

StartAddressDialog::StartAddressDialog()
{
	setupUi(this);
}

const QString& StartAddressDialog::startAddress() const
{
	return addressLineEdit->text();
}

}