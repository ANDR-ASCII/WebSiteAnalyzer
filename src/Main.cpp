#include "stdlibs.h"
#include "general_helper_functions.h"
#include "http_connection.h"
#include "menu.h"

int main()
{
    try
    {
        UserInterface::Menu menu;
    }
    catch(const std::exception& e)
    {
        std::cout << e.what();
    }

	return 0;
}
