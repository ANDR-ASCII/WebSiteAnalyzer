#pragma once

#include "stdlibs.h"

namespace Common
{

// returns a string with lower case letters
std::string strToLower(const std::string& toLowStr);

// remove white-spaces left and right
std::string cutSpaceChars(std::string& trStr);

}