#pragma once

#include "headers.h"
#include "tag.h"

namespace HtmlParser
{

class TagParserPrivate;

class TagParser
{
public:
	using iterator = std::deque<Tag>::iterator;
	using const_iterator = std::deque<Tag>::const_iterator;

	TagParser();
	TagParser(const TagParser& parser);
	TagParser& operator=(const TagParser& parser);
	const Tag& operator[](std::size_t i) const;

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;

	//! Parse whole HTML page by tags
	//! After it executed you can get each parsed tag using operator[].
	void parsePageByTags(const std::string& htmlPage);

	//! Parse specified tags from HTML page.
	//! After it executed TagParser will contain only specified parsed tags.
	//!
	//! If 'parseAttributes' flag is set then all tags will contain its attributes.
	void parseTags(const std::string& htmlPage, const std::string& tagName, bool parseAttributes = true);

	// parse specified tags with value from first argument
	void parseTagsWithValues(const std::string& htmlPage, const std::string& tagName, bool parseAttributes = true);

	std::size_t size() const;
	std::size_t countTag(const std::string& tag) const;

	void clear();
	void setInviteMode(bool setInvMode);

private:		
	// clear queue of tags if turned off inviteMode
	void testInviteMode();

	D_FUNCTION_DECL(TagParser);

private:
	bool m_inviteMode;
};
	
}