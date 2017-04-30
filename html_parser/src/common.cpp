#include "headers.h"
#include "common.h"

namespace Common
{

// remove white-spaces left and right
std::string cutSpaceChars(std::string& workStr)
{
	if (workStr.empty())
	{
		return workStr;
	}

	std::string temporaryStr;
	auto sizeOfStr = workStr.size();

	// particular case
	if (sizeOfStr == 1)
	{
		if (workStr != " ")
		{
			return workStr;
		}
		else
		{
			workStr.clear();
			return workStr;
		}
	}

	decltype(sizeOfStr) lblBegin = 0, lblEnd = 0;

	// set label of beginning string
	for (decltype(sizeOfStr) i = 0; i < sizeOfStr; ++i)
	{
		if (isspace((unsigned char)workStr[i]))
		{
			continue;
		}
		else
		{
			lblBegin = i;
			break;
		}
	}

	// set label of end string after last character
	for (auto i = sizeOfStr - 1; i; --i)
	{
		if (isspace((unsigned char)workStr[i]))
		{
			continue;
		}
		else
		{
			lblEnd = i + 1;
			break;
		}
	}

	if (lblBegin < lblEnd)
	{
		for (auto i = lblBegin; i != lblEnd; ++i)
		{
			temporaryStr += workStr[i];
		}
	}

	workStr = temporaryStr;
	return workStr;
}

// returns a string with lower case letters
std::string strToLower(const std::string& toLowStr)
{
	std::string resultStr;
	int size = toLowStr.size();

	for (int i = 0; i < size; ++i)
	{
		resultStr += static_cast<char>(tolower((unsigned char)toLowStr[i]));
	}

	return resultStr;
}

}