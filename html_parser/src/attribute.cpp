#include "attribute.h"

namespace HtmlParser
{

Attribute::Attribute(const std::string &name, const std::string &value)
	: m_attributeName(name)
	, m_attributeValue(value)
{
}

const std::string& Attribute::name() const
{
	return m_attributeName;
}

const std::string& Attribute::value() const
{
	return m_attributeValue;
}

void Attribute::setName(const std::string & setAttrName)
{
	m_attributeName = setAttrName;
}

void Attribute::setValue(const std::string & setAttrValue)
{
	m_attributeValue = setAttrValue;
}

}
