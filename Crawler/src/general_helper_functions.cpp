#include "stdlibs.h"
#include "general_helper_functions.h"

const char *hexWindows1251[] =
{
    "%80", "%81", "%82", "%83", "%84", "%85", "%86", "%87", "%88", "%89", "%8A", "%8B", "%8C", "%8D", "%8E", "%8F",
    "%90", "%91", "%92", "%93", "%94", "%95", "%96", "%97", "%98", "%99", "%9A", "%9B", "%9C", "%9D", "%9E", "%9F",
    "%A0", "%A1", "%A2", "%A3", "%A4", "%A5", "%A6", "%A7", "%A8", "%A9", "%AA", "%AB", "%AC", "%AD", "%AE", "%AF",
    "%B0", "%B1", "%B2", "%B3", "%B4", "%B5", "%B6", "%B7", "%B8", "%B9", "%BA", "%BB", "%BC", "%BD", "%BE", "%BF",
    "%C0", "%C1", "%C2", "%C3", "%C4", "%C5", "%C6", "%C7", "%C8", "%C9", "%CA", "%CB", "%CC", "%CD", "%CE", "%CF",
    "%D0", "%D1", "%D2", "%D3", "%D4", "%D5", "%D6", "%D7", "%D8", "%D9", "%DA", "%DB", "%DC", "%DD", "%DE", "%DF",
    "%E0", "%E1", "%E2", "%E3", "%E4", "%E5", "%E6", "%E7", "%E8", "%E9", "%EA", "%EB", "%EC", "%ED", "%EE", "%EF",
    "%F0", "%F1", "%F2", "%F3", "%F4", "%F5", "%F6", "%F7", "%F8", "%F9", "%FA", "%FB", "%FC", "%FD", "%FE", "%FF",
};

void General::writeBesides(const std::string &file,
                           const std::vector<std::size_t> &numsOfString,
                           const std::vector<std::string> &content)
{
    using namespace std;
    fstream out(file, ios_base::out | ios_base::trunc);

    auto size = content.size();
    for(decltype(size) i = 1; i <= size; ++i)
    {
        if(!General::findValueInVector<std::size_t>(numsOfString, i))
        {
            out << content[i - 1] << endl;
        }
    }
}

std::string General::toWindows1251(const std::string &str)
{
    std::string result;
    for(const auto &ch : str)
    {
        if(static_cast<unsigned char>(ch) > 0x7F)
        {
            result += hexWindows1251[static_cast<unsigned char>(ch) - 0x80];
        }
        else if(static_cast<unsigned char>(ch) == 0x20)
        {
            result += "%20";
        }
        else
        {
            result += ch;
        }
    }

    return result;
}
