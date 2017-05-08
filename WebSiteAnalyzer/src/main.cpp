#include "main_frame.h"
#include "application.h"

int main(int argc, char *argv[])
{
	WebSiteAnalyzer::Application a(argc, argv);

	return a.exec();
}
