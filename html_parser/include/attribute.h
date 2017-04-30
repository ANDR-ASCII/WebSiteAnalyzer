#pragma once

#include "headers.h"

namespace HtmlParser
{

class HTML_PARSER_EXPORT Attribute
{
public:
	Attribute() = default;
	Attribute(const std::string& name, const std::string& value);

	const std::string& name() const;
	const std::string& value() const;

	void setName(const std::string& attrName);
	void setValue(const std::string& attrValue);

private:
	std::string m_attributeName;
	std::string m_attributeValue;
};

}