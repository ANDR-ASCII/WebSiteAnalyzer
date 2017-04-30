#pragma once

#include "headers.h"
#include "tag.h"

namespace HtmlParser
{

class Attribute;

class TagParserPrivate
{
public:
	bool isDoubleQuote(char ch) const;

	bool isSingleQuote(char ch) const;

	bool isEqualCharacter(char ch) const;

	bool isOpenTag(char ch) const;

	bool isCloseTag(char ch) const;

	bool isNewLine(char ch) const;

	bool isSpace(char ch) const;

	bool isAlpha(char ch) const;

	bool isAlnum(char ch) const;

	bool isSetTagAttributes(const std::string& tagString) const;

	bool isSingleTag(const std::string& tagName) const;

	std::string::size_type isSetTagAttribute(const std::string& tagString, const std::string& attribute) const;

	// moves the pointer to the comment tag
	void skipCommentaryTag(const std::string& htmlPage, std::size_t& position) const;

	// true if the text starting at position "position" occurs characters !--
	bool isCommentaryTag(const std::string& htmlPage, std::size_t& position) const;

	// reads starting at position "position" first occur tag
	// <tag attr="val">value of tag</tag> returns string following: [tag attribute="value"] without "[" and ]""
	// pointer "position" moves to the next character by character ">"
	std::string readTag(const std::string& htmlPage, std::size_t& position) const;

	// reads all characters starting at the position "position"
	// up to the first character of open/close any occurrence tag
	// pointer sets on the first occurrence character '<'
	std::string readValue(const std::string& htmlPage, std::size_t& position) const;

	// reads all characters starting at the position "position" up to the specified tag
	// returns all read characters, pointer moves after tag, before which should be to read
	std::string readAllUpToTag(const std::string& htmlPage, const std::string& tag, std::size_t& position) const;

	// receives tag string like this - (tagname attribute1="value1" attribute2="value2")
	// returns value of specified attribute
	std::string attributeOfTag(const std::string& tagString, const std::string& attribute) const;

	// receives string like this: (tagname [attribute1="value1" attribute2="value2"])
	// return tagname
	std::string tagName(const std::string& tagString) const;

	// reads all attributes of passed tag and returns vector of attributes
	// expects string like this: (tagname [attribute1="value1" attribute2="value2"])
	// attributes can be any number
	std::deque<Attribute> readAllTagAttributes(const std::string& tagString) const;

	std::deque<Tag>& internalTagStorage();
	std::deque<Tag> const& internalTagStorage() const;

private:
	static const std::string s_singleTags[];
	static const std::size_t s_singleTagsNumber;
	static const std::size_t s_lowestStartAttributeNamePosition;
	static const std::string s_openHtmlCommentaryTagName;
	static const std::string s_closeHtmlCommentaryTag;
	static const char s_equalCharacter;
	static const char s_openTagCharacter;
	static const char s_closeTagCharacter;
	static const char s_doubleQuoteCharacter;
	static const char s_singleQuoteCharacter;
	static const char s_newLineCharacter;
	
	std::deque<Tag> m_tags;

private:
	void skipWhitespaces(const std::string& str, std::size_t& position) const;

	template <typename Pred>
	std::string readUntil(const std::string& str, std::size_t& position, Pred p) const
	{
		const std::size_t stringLength = str.size();

		std::size_t idx = position;
		std::string accumulator;

		for (; idx < stringLength; ++idx)
		{
			if (p(str[idx]))
			{
				accumulator += str[idx];
			}
			else
			{
				break;
			}
		}

		position = idx;

		return accumulator;
	}
};

}