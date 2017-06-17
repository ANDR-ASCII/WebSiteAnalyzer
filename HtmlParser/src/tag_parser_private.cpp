#include "headers.h"
#include "attribute.h"
#include "tag_parser_private.h"
#include "common.h"

namespace HtmlParser
{

const std::string TagParserPrivate::s_singleTags[] =
{
	"area",
	"base",
	"basefont",
	"bgsound",
	"br",
	"col",
	"command",
	"embed",
	"frame",
	"hr",
	"img",
	"input",
	"isindex",
	"keygen",
	"source",
	"track",
	"wbr",
	"link",
	"meta",
	"param",
	"!doctype"
};

const std::size_t TagParserPrivate::s_singleTagsNumber = sizeof(s_singleTags) / sizeof(s_singleTags[0]);

const std::size_t TagParserPrivate::s_lowestStartAttributeNamePosition = 2;

const std::string TagParserPrivate::s_openHtmlCommentaryTagName = "!--";

const std::string TagParserPrivate::s_closeHtmlCommentaryTag = "-->";

const char TagParserPrivate::s_equalCharacter = '=';

const char TagParserPrivate::s_openTagCharacter = '<';

const char TagParserPrivate::s_closeTagCharacter = '>';

const char TagParserPrivate::s_doubleQuoteCharacter = '"';

const char TagParserPrivate::s_singleQuoteCharacter = '\'';

const char TagParserPrivate::s_newLineCharacter = '\n';

/*********************************************************************************************************/

void TagParserPrivate::skipWhitespaces(const std::string& str, std::size_t& position) const
{
	const std::size_t stringLength = str.size();
	std::size_t idx = position;

	for (; idx < stringLength; ++idx)
	{
		if (!isSpace(str[idx]))
		{
			break;
		}
	}
	position = idx;
}

bool TagParserPrivate::isDoubleQuote(char ch) const
{
	if (ch == s_doubleQuoteCharacter)
	{
		return true;
	}

	return false;
}

bool TagParserPrivate::isSingleQuote(char ch) const
{
	if (ch == s_singleQuoteCharacter)
	{
		return true;
	}

	return false;
}

bool TagParserPrivate::isEqualCharacter(char ch) const
{
	if (ch == s_equalCharacter)
	{
		return true;
	}

	return false;
}

bool TagParserPrivate::isOpenTag(char ch) const
{
	if (ch == s_openTagCharacter)
	{
		return true;
	}

	return false;
}

bool TagParserPrivate::isCloseTag(char ch) const
{
	if (ch == s_closeTagCharacter)
	{
		return true;
	}

	return false;
}

bool TagParserPrivate::isNewLine(char ch) const
{
	if (ch == s_newLineCharacter)
	{
		return true;
	}

	return false;
}

#pragma warning(push)
#pragma warning(disable: 4800)

bool TagParserPrivate::isSpace(char ch) const
{
	return std::isspace(static_cast<unsigned char>(ch));
}

bool TagParserPrivate::isAlpha(char ch) const
{
	return std::isalpha(static_cast<unsigned char>(ch));
}

bool TagParserPrivate::isAlnum(char ch) const
{
	return std::isalnum(static_cast<unsigned char>(ch));
}

#pragma warning(pop)

void TagParserPrivate::skipCommentaryTag(const std::string& htmlPage, std::size_t& position) const
{
	if (!isCommentaryTag(htmlPage, position))
	{
		return;
	}

	position += s_openHtmlCommentaryTagName.size();
	const std::size_t closeCommentaryTagPosition = htmlPage.find(s_closeHtmlCommentaryTag, position);

	position = closeCommentaryTagPosition != std::string::npos ? closeCommentaryTagPosition + s_closeHtmlCommentaryTag.size() :
		throw std::logic_error("Cannot find closed commentary tag");
}	
	
bool TagParserPrivate::isCommentaryTag(const std::string& htmlPage, std::size_t& position) const
{
	if (position > htmlPage.size() || htmlPage.empty())
	{
		throw std::invalid_argument("Empty argument or position greater than size of string");
	}

	if (htmlPage.find(s_openHtmlCommentaryTagName, position) != std::string::npos)
	{
		return true;
	}
	
	return false;
}	
	
std::string TagParserPrivate::readTag(const std::string& htmlPage, std::size_t& position) const
{	
	const std::size_t openTagCharacterPosition = htmlPage.find(s_openTagCharacter, position);

	if (openTagCharacterPosition == std::string::npos)
	{
		return std::string();
	}

	const std::size_t closeTagCharacterPosition = htmlPage.find(s_closeTagCharacter, openTagCharacterPosition);

	if (closeTagCharacterPosition == std::string::npos)
	{
		return std::string();
	}

	// move pointer to the next character after close tag character position
	position = closeTagCharacterPosition + 1;

	// +1 for ignoring '<' character
	return htmlPage.substr(openTagCharacterPosition + 1, closeTagCharacterPosition - openTagCharacterPosition - 1);
}

//
// TODO: refactor
//
std::string TagParserPrivate::readValue(const std::string& htmlPage, std::size_t& position) const
{
	std::string value;
	const std::size_t size = htmlPage.size();

	while (position < size)
	{
		if (!isOpenTag(htmlPage[position]))
		{
			value += htmlPage[position++];
		}
		else
		{
			break;
		}
	}

	return value;
}

//
// TODO: refactor
//
std::string TagParserPrivate::readAllUpToTag(const std::string& htmlPage, const std::string& tag, std::size_t& position) const
{
	std::string value = readValue(htmlPage, position);
	std::string currentTagName = Common::strToLower(readTag(htmlPage, position));

	const std::size_t size = htmlPage.size();

	while (position < size && tagName(currentTagName) != Common::strToLower(tag))
	{
		currentTagName = s_openTagCharacter + currentTagName + s_closeTagCharacter;
		value += currentTagName;

		value += readValue(htmlPage, position);
		currentTagName = Common::strToLower(readTag(htmlPage, position));
	}

	return value;
}

std::string TagParserPrivate::attributeOfTag(const std::string& tagString, const std::string& attribute) const
{
	const std::size_t idx = isSetTagAttribute(tagString, attribute);

	const bool isArgumentInvalid = tagString.empty() || attribute.empty();
	const bool isAttributeFound = idx != std::string::npos;

	std::string attributeValue;

	if (isArgumentInvalid)
	{
		throw std::invalid_argument("Empty argument");
	}

	if (isAttributeFound)
	{
		const std::size_t attributeLength = attribute.size();
		const std::size_t lastCharacterAttributeNamePosition = idx + attributeLength;
		const std::size_t tagStringLength = tagString.size();

		std::size_t idx = lastCharacterAttributeNamePosition;

		skipWhitespaces(tagString, idx);

		//
		// check validity of HTML attribute syntax
		// and increment position to the next character
		// which is should be alphabetical character or 
		// single/double quotes character
		//
		if (!isEqualCharacter(tagString[idx++]))
		{
			throw std::logic_error("Tag string do not contain '=' character after attribute name");
		}

		skipWhitespaces(tagString, idx);

		if (!(isDoubleQuote(tagString[idx]) || isSingleQuote(tagString[idx])))
		{
			attributeValue = readUntil(tagString, idx, [this](char ch) { return !isSpace(ch); });
		}
		else
		{
			//**
			//***********************************************************************************************************
			//**

			// I seem this is a bad style programming
			// but I suppose this code will see only am I

			using checkCharacterFunctionTypePointer = bool(TagParserPrivate::*)(char) const;

			checkCharacterFunctionTypePointer checkQuotesFunction = isDoubleQuote(tagString[idx]) ?
				&TagParserPrivate::isDoubleQuote : &TagParserPrivate::isSingleQuote;

			//**
			//***********************************************************************************************************
			//**

			attributeValue = readUntil(tagString, ++idx,
				[this, checkQuotesFunction](char ch) { return !(this->*checkQuotesFunction)(ch); });
		}

		return attributeValue;
	}

	// attribute not found
	return std::string();
}

std::string TagParserPrivate::tagName(const std::string& tagString) const
{
	const std::size_t size = tagString.size();

	if (tagString.empty())
	{
		return std::string();
	}

	std::size_t position = 0;

	return readUntil(tagString, position, [this](char ch) { return !isSpace(ch); });
}

std::size_t TagParserPrivate::isSetTagAttribute(const std::string& tagString, const std::string& attribute) const
{
	const std::size_t idx = tagString.find(attribute);

	const bool isAttributeFindAndPositionCorrect = std::string::npos &&
		idx >= s_lowestStartAttributeNamePosition &&
		isSpace(tagString[idx - 1]);

	return isAttributeFindAndPositionCorrect ? idx : std::string::npos;
}

std::deque<Attribute> TagParserPrivate::readAllTagAttributes(const std::string& tagString) const
{
	const std::size_t tagStringLength = tagString.size();
	const std::size_t tagNameLength = tagName(tagString).size();

	std::deque<Attribute> attributes;

	// if exists attributes, read it
	if (isSetTagAttributes(tagString))
	{
		Attribute attribute;

		std::size_t idx = tagNameLength;

		skipWhitespaces(tagString, idx);

		while(idx < tagStringLength)
		{
			// write attribute name
			attribute.setName(readUntil(tagString, idx, [this](char ch) { return !isSpace(ch) && !isEqualCharacter(ch); }));

			if (attribute.name() == "/")
			{
				attribute.setName(std::string());
				continue;
			}

			skipWhitespaces(tagString, idx);

			//
			// check validity of HTML attribute syntax
			// and increment position to the next character
			// which is should be alphabetical character or 
			// single/double quotes character
			//
			if (!isEqualCharacter(tagString[idx++]))
			{
				throw std::logic_error("Tag string do not contain '=' character after attribute name");
			}

			skipWhitespaces(tagString, idx);

			if (!(isDoubleQuote(tagString[idx]) || isSingleQuote(tagString[idx])))
			{
				attribute.setValue(readUntil(tagString, idx, [this](char ch) { return !isSpace(ch); }));
			}
			else
			{
				//**
				//***********************************************************************************************************
				//**

				// I seem this is a bad style programming
				// but I suppose this code will see only am I

				using checkCharacterFunctionTypePointer = bool(TagParserPrivate::*)(char) const;

				checkCharacterFunctionTypePointer checkQuotesFunction = isDoubleQuote(tagString[idx]) ?
					&TagParserPrivate::isDoubleQuote : &TagParserPrivate::isSingleQuote;

				//**
				//***********************************************************************************************************
				//**

				attribute.setValue(readUntil(tagString, ++idx,
					[this, checkQuotesFunction](char ch) { return !(this->*checkQuotesFunction)(ch); }));

				++idx;
			}

			skipWhitespaces(tagString, idx);

			attributes.push_back(attribute);
		}
	}

	return attributes;
}

std::deque<Tag>& TagParserPrivate::internalTagStorage()
{
	return m_tags;
}

std::deque<HtmlParser::Tag> const& TagParserPrivate::internalTagStorage() const
{
	return m_tags;
}

//
// TODO: refactor
//
bool TagParserPrivate::isSetTagAttributes(const std::string& tagString) const
{
	const std::size_t idx = tagString.find(s_equalCharacter);
	std::size_t i = 0;

	if (idx != std::string::npos)
	{
		i = idx - 1;

		if (idx <= 2)
		{
			return false;
		}

		// skip all white-spaces between character = and last letter of name attribute
		do
		{
			--i;

		} while (isSpace(tagString[i]) && i > 0);

		// skip name of attribute until meet white-space characters
		do
		{
			--i;

		} while (!isSpace(tagString[i]) && i > 0);

		// test second part after character =
		if (isSpace(tagString[i]))
		{
			i = idx + 1;

			// if after = meet " or ' or letter/digit, return true
			if (isDoubleQuote(tagString[i]) || isSingleQuote(tagString[i]) || isAlnum(tagString[i]))
			{
				return true;
			}

			// skip white-spaces
			do
			{
				++i;
			} while (isSpace(tagString[i]) && i > 0);

			// if find double or single quotes or letter/digit, return true
			if (isDoubleQuote(tagString[i]) || isSingleQuote(tagString[i]) || isAlnum(tagString[i]))
			{
				return true;
			}
			// otherwise tag invalid
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool TagParserPrivate::isSingleTag(const std::string& tagName) const
{
	for (std::size_t i = 0; i < s_singleTagsNumber; i++)
	{
		if (Common::strToLower(tagName) == s_singleTags[i])
		{
			return true;
		}
	}

	return false;
}

}