#include "main_frame.h"
#include "application.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	WebSiteIndexer::Application a(argc, argv);

	return a.exec();
}
