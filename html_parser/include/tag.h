#pragma once

#include "headers.h"
#include "attribute.h"

namespace HtmlParser
{

class HTML_PARSER_EXPORT Tag
{
public:
	const std::string& name() const;
	const std::string& value() const;
	const std::deque<Attribute>& allAttributes() const;
	
	//
	//! throws exception if error
	//! returns empty value if specified attribute name not found
	//
	const std::string attribute(const std::string& nameAttribute) const;

	void setName(const std::string& setTagName);
	void setValue(const std::string& setValueArg);

	void setAttribute(const std::string& attributeName, const std::string& attributeValue);
	void setAttributes(const std::deque<Attribute>& setAttributes);

private:
	std::string m_tagName;
	std::string m_tagValue;
	std::deque<Attribute> m_attributes;
};

}