#pragma once

#include "headers.h"

namespace Common
{

// returns a string with lower case letters
std::string HTML_PARSER_EXPORT strToLower(const std::string& toLowStr);

// remove white-spaces left and right
std::string HTML_PARSER_EXPORT cutSpaceChars(std::string& trStr);

}