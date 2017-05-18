#include "headers.h"
#include "attribute.h"
#include "tag.h"
#include "tag_parser.h"
#include "tag_parser_private.h"
#include "common.h"

namespace HtmlParser
{

D_FUNCTION_IMPL(TagParser)


TagParser::TagParser()
	: m_inviteMode(false)
{
}

TagParser::TagParser(const TagParser& parser)
{
	D_FUNCTION(TagParser);

	m_inviteMode = parser.m_inviteMode;
	clear();

	const auto tagsNumber = parser.size();

	for (auto i = 0ul; i < tagsNumber; ++i)
	{
		d_pointer->internalTagStorage().push_back(parser[i]);
	}
}

TagParser::iterator TagParser::begin()
{
	D_FUNCTION(TagParser);

	return d_pointer->internalTagStorage().begin();
}

TagParser::iterator TagParser::end()
{
	D_FUNCTION(TagParser);

	return d_pointer->internalTagStorage().end();
}

TagParser::const_iterator TagParser::begin() const
{
	D_FUNCTION(TagParser);

	return d_pointer->internalTagStorage().begin();
}

TagParser::const_iterator TagParser::end() const
{
	D_FUNCTION(TagParser);

	return d_pointer->internalTagStorage().end();
}

TagParser::const_iterator TagParser::cbegin() const
{
	D_FUNCTION(TagParser);

	return d_pointer->internalTagStorage().cbegin();
}

TagParser::const_iterator TagParser::cend() const
{
	D_FUNCTION(TagParser);

	return d_pointer->internalTagStorage().cend();
}

TagParser& TagParser::operator=(const TagParser& parser)
{
	D_FUNCTION(TagParser);

	if (this == &parser)
	{
		return *this;
	}

	m_inviteMode = parser.m_inviteMode;
	clear();

	const auto tagsNumber = parser.size();

	for (auto i = 0ul; i < tagsNumber; ++i)
	{
		d_pointer->internalTagStorage().push_back(parser[i]);
	}

	return *this;
}

void TagParser::parsePageByTags(const std::string& htmlPage)
{
	D_FUNCTION(TagParser);

	testInviteMode();

	if (htmlPage.empty())
	{
		throw std::invalid_argument("Empty argument");
	}

	std::string tag;
	std::string tagValue;

	const std::size_t lengthOfPage = htmlPage.size();

	std::size_t pointer = 0u;
	std::size_t mem = 0u;

	while (pointer < lengthOfPage)
	{
		Tag tmpTag;
		Attribute tmpAttr;

		tag = d_pointer->readTag(htmlPage, pointer);

		tmpTag.setName(Common::strToLower(d_pointer->tagName(tag)));
		tmpTag.setAttributes(d_pointer->readAllTagAttributes(tag));

		if (tag[0] != '/')
		{
			if (!d_pointer->isSingleTag(d_pointer->tagName(tag)))
			{
				mem = pointer;
				tagValue = d_pointer->readAllUpToTag(htmlPage, "/" + d_pointer->tagName(tag), pointer);
				tmpTag.setValue(tagValue);
				pointer = mem;
			}

			d_pointer->internalTagStorage().push_back(tmpTag);
		}
	}
}

// parse specified tags from first argument
void TagParser::parseTags(const std::string& htmlPage, const std::string& tagName, bool parseAttributes)
{
	D_FUNCTION(TagParser);

	testInviteMode();

	if (tagName.empty() || htmlPage.empty() || tagName == "!--")
	{
		return;
	}

	decltype(htmlPage.size()) position = 0, size = htmlPage.size();

	// text of read the tag like this - tag [attribute1="value1" attribute2="value2"]
	std::string tagText;
	std::deque<HtmlParser::Attribute> attributesOfNameTag;

	// name of current read tag
	std::string nameOfReadedTag, nameTagCmp = Common::strToLower(tagName);


	while (position < size)
	{
		// initializing object of Tag
		Tag tmpTag;
		tagText = d_pointer->readTag(htmlPage, position);

		if (Common::strToLower(d_pointer->tagName(tagText)).empty())
		{
			break;
		}

		// if find the specified tag, write it
		if (Common::strToLower(d_pointer->tagName(tagText)) == nameTagCmp)
		{
			tmpTag.setName(nameTagCmp);

			// if need parse attributes
			if (parseAttributes)
			{
				try
				{
					tmpTag.setAttributes(d_pointer->readAllTagAttributes(tagText));
				}
				catch (const std::logic_error&)
				{
					//
					// TODO
					//
				}
			}

			d_pointer->internalTagStorage().push_back(tmpTag);
		}
	}
}

// parse specified tags with value from first argument
void TagParser::parseTagsWithValues(const std::string& htmlPage, const std::string& tagName, bool parseAttributes)
{
	D_FUNCTION(TagParser);

	testInviteMode();

	if (tagName.empty() || htmlPage.empty() || tagName == "!--")
	{
		return;
	}

	std::size_t position = 0, size = htmlPage.size();

	std::string tagText, nameTagCmp = Common::strToLower(tagName);
	std::deque<HtmlParser::Attribute> attributesOfNameTag;

	while (position < size)
	{
		Tag tmpTag;
		tagText = d_pointer->readTag(htmlPage, position);

		if (Common::strToLower(d_pointer->tagName(tagText)).empty())
		{
			break;
		}

		if (Common::strToLower(d_pointer->tagName(tagText)) == nameTagCmp)
		{
			tmpTag.setName(nameTagCmp);

			// if need parse attributes of tag
			if (parseAttributes)
			{
				try
				{
					tmpTag.setAttributes(d_pointer->readAllTagAttributes(tagText));
				}
				catch (const std::logic_error&)
				{
					//
					// TODO
					//
				}
			}

			tmpTag.setValue(d_pointer->readAllUpToTag(htmlPage, "/" + d_pointer->tagName(tagText), position));

			d_pointer->internalTagStorage().push_back(tmpTag);
		}
	}
}

Tag const& TagParser::operator[](std::size_t i) const
{
	D_FUNCTION(TagParser);

	std::size_t countTags = d_pointer->internalTagStorage().size();

	if (i > countTags || !countTags)
	{
		throw std::out_of_range("Error indexing array");
	}

	return d_pointer->internalTagStorage()[i];
}

std::size_t TagParser::size() const
{
	D_FUNCTION(TagParser);

	return d_pointer->internalTagStorage().size();
}

std::size_t TagParser::countTag(const std::string& tag) const
{
	D_FUNCTION(TagParser);

	std::size_t number = size(), numberOfTagsFound = 0;

	if (number > 0)
	{
		for (std::size_t i = 0; i < number; ++i)
		{
			if (d_pointer->internalTagStorage()[i].name() == tag)
			{
				numberOfTagsFound++;
			}
		}

		return numberOfTagsFound;
	}
	else
	{
		return 0;
	}
}

// clear queue of tags if turned off inviteMode
void TagParser::testInviteMode()
{
	if (!m_inviteMode)
	{
		clear();
	}
}

void TagParser::clear()
{
	D_FUNCTION(TagParser);

	d_pointer->internalTagStorage().clear();
}

void TagParser::setInviteMode(bool inviteMode)
{
	m_inviteMode = inviteMode;
}

}