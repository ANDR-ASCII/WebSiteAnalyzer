#include "stdlibs.h"
#include "general_helper_functions.h"
#include "menu.h"
#include "common.h"

const boost::regex UserInterface::Menu::getNumbers("^\\s*\\d+(?:\\s*,\\s*\\d+)*$");

UserInterface::Menu::Menu()
{
    crawl.setViewer(&ccv);
    printMenu();
    interrogation();
}

void UserInterface::Menu::interrogation()
{
    using namespace std;
    int command = -1;

    while (true)
    {
        bool validCmd = false;
        cout << "\nInput the command: ";

        while (!validCmd)
        {
            while (!(cin >> command))
            {
                cin.clear();
                string line;
                getline(cin, line);

                if(command == 0)
                {
                    cout << "\nSorry, but '" << line << "' is not a number.\n";
                    cout << "Input the valid command: ";
                }
                else
                {
                    break;
                }
            }

            if (command < 0 || command >= AMOUNT_HANDLERS)
            {
                cout << "\nSorry, but command '" << command << "' is undefined.\n";
                cout << "Input the valid command: ";
            }
            else
            {
                validCmd = true;
            }
        }

        (this->*runHandler[command])();
    }
}

void UserInterface::Menu::printMenu()
{
    using namespace std;
    string border = "****************************************************************************";

    cout << border;

    cout.width(border.size() - 1); cout.fill(' ');
    cout << left << endl <<  "|" << right << "|";
    cout.width(border.size() - 1); cout.fill(' ');
    cout << left << endl <<  "|" << right << "|";
    cout.width(border.size() - 1); cout.fill('_');
    cout << left << endl << "| 0. For exit from program." << right << "|";


    cout.width(border.size() - 1); cout.fill(' ');
    cout << left << endl <<  "|" << right << "|";
    cout.width(border.size() - 1); cout.fill('_');
    cout << left << endl <<  "| 1. Start." << right << "|";

    cout.width(border.size() - 1); cout.fill(' ');
    cout << left << endl <<  "|" << right << "|";
    cout.width(border.size() - 1); cout.fill('_');
    cout << left << endl <<  "| 2. Clear console window." << right << "|";

    cout.width(border.size() - 1); cout.fill(' ');
    cout << left << endl <<  "|" << right << "|";
    cout.width(border.size() - 1); cout.fill('_');
    cout << left << endl << "| 3. Set start address." << right << "|";

    cout.width(border.size() - 1); cout.fill(' ');
    cout << left << endl <<  "|" << right << "|";
    cout.width(border.size() - 1); cout.fill('_');
    cout << left << endl <<  "| 4. Set pause between HTTP requests(3000 milliseconds by default)." << right << "|";

    cout.width(border.size() - 1); cout.fill(' ');
    cout << left << endl <<  "|" << right << "|";
    cout.width(border.size() - 1); cout.fill('_');
    cout << left << endl <<  "| 5. Set signature which will be displayed in User-Agent." << right << "|";

    cout.width(border.size() - 1); cout.fill(' ');
    cout << left << endl <<  "|" << right << "|";
    cout.width(border.size() - 1); cout.fill('_');
    cout << left << endl <<  "| 6. Set max depth(0 is unlimited)." << right << "|";

    cout.width(border.size() - 1); cout.fill(' ');
    cout << left << endl <<  "|" << right << "|";
    cout.width(border.size() - 1); cout.fill('_');
    cout << left << endl <<  "| 7. Turn on/off infinite crawl." << right << "|";

    cout.width(border.size() - 1); cout.fill(' ');
    cout << left << endl <<  "|" << right << "|";
    cout.width(border.size() - 1); cout.fill('_');
    cout << left << endl <<  "| 8. Set path to save pages(choice directory)." << right << "|";

    cout.width(border.size() - 1); cout.fill(' ');
    cout << left << endl <<  "|" << right << "|";
    cout.width(border.size() - 1); cout.fill(' ');
    cout << left << endl <<  "|" << right << "|";

    cout << endl << border << endl << endl;
}

// handlers
void UserInterface::Menu::quit()
{
    exit(EXIT_SUCCESS);
}

void UserInterface::Menu::start()
{
    using namespace std;

    eatline();
    string ready;

    cout << "\nCrawler will be started with configurations:\n\n"
            "Start address: " << crawl.getStartAddress() << endl <<
            "User-Agent: " << crawl.getSignature() << endl <<
            "Pause time: " << crawl.getTimePause() << MEASURE_OF_TIME << endl <<
            "Max Depth: " << crawl.getMaxDepth() << " links" << endl;

    if (crawl.getModeInfiniteCrawl())
    {
        cout << "Infinite crawl mode turned on.\n";
    }
    else
    {
        cout << "Infinite crawl mode turned off.\n";
    }

    cout << "\nCase-insensitive 'q', 'quit' - exit from program\n"
            "Case-insensitive 'r', 'ret', 'return' - return to menu\n";

    if (crawl.readyForStart())
    {
        cout << "After starting will not be able to stop the program until it is finished.\n\nRun?(y/n): ";

        while(true)
        {
            getline(cin, ready);

            if (Common::strToLower(ready) == "q" || 
				Common::strToLower(ready) == "quit")
            {
                exit(0);
            }

            if (Common::strToLower(ready) == "r" || 
				Common::strToLower(ready) == "ret" || 
				Common::strToLower(ready) == "return" || 
				Common::strToLower(ready) == "n")
            {
                break;
            }

            if (Common::strToLower(ready) != "y")
            {
                cout << "Invalid value. Please re-enter: ";
                continue;
            }

            if (Common::strToLower(ready) == "y")
            {
                crawl.start();
                break;
            }
        }
    }
    else
    {
        cout << "Crawler don't ready for start.\n";
        cin.get();
    }
}

void UserInterface::Menu::clearScreen()
{
    system(CLEAR_SCREEN);
    printMenu();
}

void UserInterface::Menu::setStartAddress()
{
    using namespace std;

    eatline();
    bool successful = false;
    string address;

    while(!successful)
    {
        cout << "\nCase-insensitive 'q', 'quit' - exit from program\n"
                "Case-insensitive 'r', 'ret', 'return' - return to menu\n"
                "Please input valid address: ";

        while(!getline(cin, address))
        {
            cin.clear();
        }

        if (Common::strToLower(address) == "q" || 
			Common::strToLower(address) == "quit")
        {
            exit(0);
        }

        if (Common::strToLower(address) == "r" || Common::strToLower(address) == "ret" || 
			Common::strToLower(address) == "return")
        {
            return;
        }

        startLnk = address;
        successful = startLnk.isAbsoluteAddress();

        if (!successful)
        {
            cout << "Invalid address!\n";
        }
    }

    if (Common::strToLower(address) == "r" || 
		Common::strToLower(address) == "ret" || 
		Common::strToLower(address) == "return")
    {
        return;
    }

    cout << "Start address was set. \n";
    crawl.setStartAddress(address);
}

void UserInterface::Menu::setPauseBetweenReqs()
{
    using namespace std;

    cout << "\nCase-insensitive 'q', 'quit' - exit from program\n"
            "Case-insensitive 'r', 'ret', 'return' - return to menu\n"
            "Input pause time between HTTP requests in" MEASURE_OF_TIME ": ";

    unsigned timePause = -1; // big number
    string line;

    while (!(cin >> timePause))
    {
        cin.clear();
        getline(cin, line);

        if (!timePause)
        {
            if (Common::strToLower(line) == "q" || 
				Common::strToLower(line) == "quit")
            {
                exit(0);
            }

            if (Common::strToLower(line) == "r" ||
				Common::strToLower(line) == "ret" ||
				Common::strToLower(line) == "return")
            {
                return;
            }

            cout << "\nSorry, but '" << line << "' is not a number.\n";
            cout << "Input the valid command: ";
        }
        else
        {
            return;
        }
    }

    if (Common::strToLower(line) == "r" ||
		Common::strToLower(line) == "ret" ||
		Common::strToLower(line) == "return")
    {
        return;
    }

    cout << "Pause time equal " << timePause << MEASURE_OF_TIME " was set.\n";
    crawl.setPause(timePause);
}

void UserInterface::Menu::setUserAgent()
{
    using namespace std;

    eatline();
    string signature;

    cout << "\nCase-insensitive 'q', 'quit' - exit from program\n"
            "Case-insensitive 'r', 'ret', 'return' - return to menu\n"
            "Please input signature: ";

    while (!getline(cin, signature))
    {
        cin.clear();
    }

    if (Common::strToLower(signature) == "q" || 
		Common::strToLower(signature) == "quit")
    {
        exit(0);
    }

    if (Common::strToLower(signature) == "r" || 
		Common::strToLower(signature) == "ret" || 
		Common::strToLower(signature) == "return")
    {
        return;
    }

    cout << "Signature: '" << signature << "' was set.\n";
    crawl.setSignature(signature);
}

void UserInterface::Menu::setMaxDepth()
{
    using namespace std;

    cout << "\nCase-insensitive 'q', 'quit' - exit from program\n"
            "Case-insensitive 'r', 'ret', 'return' - return to menu\n"
            "Input max depth: ";

    size_t depth = -1; // big number
    string line;

    while (!(cin >> depth))
    {
        cin.clear();
        getline(cin, line);

        if (!depth)
        {
            if (Common::strToLower(line) == "q" || 
				Common::strToLower(line) == "quit")
            {
                exit(0);
            }

            if (Common::strToLower(line) == "r" || 
				Common::strToLower(line) == "ret" || 
				Common::strToLower(line) == "return")
            {
                return;
            }

            cout << "\nSorry, but '" << line << "' is not a number.\n";
            cout << "Input the valid command: ";
        }
        else
        {
            return;
        }
    }

    cout << "Max depth equal " << depth << " links was set.\n";
    crawl.setMaxDepth(depth);
}

void UserInterface::Menu::switchInfiniteMode()
{
    using namespace std;

    turnOnOffInfCrawl = !turnOnOffInfCrawl;
    if(turnOnOffInfCrawl)
    {
        cout << "Infinite crawl mode turned on.\n";
    }
    else
    {
        cout << "Infinite crawl mode turned off.\n";
    }

    crawl.setModeInfiniteCrawl(turnOnOffInfCrawl);
}

void UserInterface::Menu::setPathToSave()
{
    using namespace std;

    eatline();
    string path;

    cout << "\nCase-insensitive 'q', 'quit' - exit from program\n"
            "Case-insensitive 'r', 'ret', 'return' - return to menu\n"
            "Please input path: ";

    while (!getline(cin, path))
    {
        cin.clear();
    }

    if (Common::strToLower(path) == "q" || 
		Common::strToLower(path) == "quit")
    {
        exit(0);
    }

    if (Common::strToLower(path) == "r" ||
		Common::strToLower(path) == "ret" ||
		Common::strToLower(path) == "return")
    {
        return;
    }

    cout << "Path: '" << path << "' was set.\n";
    crawl.setPathToSavePages(path);
}
