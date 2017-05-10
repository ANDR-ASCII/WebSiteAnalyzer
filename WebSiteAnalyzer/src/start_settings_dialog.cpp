#include "stdlibs.h"
#include "start_settings_dialog.h"
#include "url.h"

namespace WebSiteAnalyzer
{

StartSettingsDialog::StartSettingsDialog(QWidget* parent)
	: QDialog(parent)
{
	setupUi(this);

	connect(startButton, SIGNAL(clicked()), SLOT(slot_checkAcceptedData()));
	connect(this, SIGNAL(close()), SLOT(reject()));
}

QString StartSettingsDialog::startAddress() const
{
	return addressLineEdit->text();
}

void StartSettingsDialog::slot_checkAcceptedData()
{
	HtmlParser::Url url = startAddress().toStdString();

	if (!url.isAbsoluteAddress())
	{
		QTextCodec* codec = QTextCodec::codecForLocale();

		QMessageBox msgBox;
		msgBox.setText(codec->toUnicode("Неправильный URL"));
		msgBox.exec();
	}
	else
	{
		accept();
	}
}

std::size_t StartSettingsDialog::maxCrawlUrls() const
{
	return maxCrawlUrlsSpinbox->value();
}

std::size_t StartSettingsDialog::requestPause() const
{
	return timePauseSpinbox->value();
}

}
