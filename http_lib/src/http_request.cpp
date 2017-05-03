#include "stdlibs.h"
#include "http_request.h"

HttpLib::HttpRequest::HttpRequest()
    : m_requestVersion(Version::Version1_1)
    , m_requestMethod(Method::Get)
    , m_requestConnection(ConnectionType::KeepAlive)
{
}

HttpLib::HttpRequest::HttpRequest(const HttpRequest &copyRequest)
{
	m_compiledRequest = copyRequest.m_compiledRequest;
	m_requestConnection = copyRequest.m_requestConnection;
	m_requestHost = copyRequest.m_requestHost;
	m_requestMethod = copyRequest.m_requestMethod;
	m_requestPath = copyRequest.m_requestPath;
	m_requestUserAgent = copyRequest.m_requestUserAgent;
	m_requestVersion = copyRequest.m_requestVersion;
}

HttpLib::HttpRequest &HttpLib::HttpRequest::operator=(const HttpRequest &copyRequest)
{
	if (*this == copyRequest)
		return *this;

	m_compiledRequest = copyRequest.m_compiledRequest;
	m_requestConnection = copyRequest.m_requestConnection;
	m_requestHost = copyRequest.m_requestHost;
	m_requestMethod = copyRequest.m_requestMethod;
	m_requestPath = copyRequest.m_requestPath;
	m_requestUserAgent = copyRequest.m_requestUserAgent;
	m_requestVersion = copyRequest.m_requestVersion;

	return *this;
}

bool HttpLib::HttpRequest::operator==(const HttpRequest &cmpRequest)
{
	if (m_compiledRequest == cmpRequest.m_compiledRequest &&
		m_requestConnection == cmpRequest.m_requestConnection &&
		m_requestHost == cmpRequest.m_requestHost &&
		m_requestMethod == cmpRequest.m_requestMethod &&
		m_requestPath == cmpRequest.m_requestPath &&
		m_requestUserAgent == cmpRequest.m_requestUserAgent &&
		m_requestVersion == cmpRequest.m_requestVersion)

			return true;

	return false;
}

bool HttpLib::HttpRequest::operator!=(const HttpRequest &cmpRequest)
{
	if (*this == cmpRequest)
		return false;

	return true;
}

void HttpLib::HttpRequest::setMethod(const Method method)
{
	m_requestMethod = method;
}

void HttpLib::HttpRequest::setRelativePath(const std::string &pathToRes)
{
	m_requestPath = pathToRes;
}

void HttpLib::HttpRequest::setHttpVersion(Version httpVersion)
{
	m_requestVersion = httpVersion;
}

void HttpLib::HttpRequest::setHost(const std::string &host)
{
	m_requestHost = host;
}

void HttpLib::HttpRequest::setUserAgent(const std::string &userAgent)
{
	m_requestUserAgent = userAgent;
}

void HttpLib::HttpRequest::setConnectionType(const ConnectionType connectionType)
{
	m_requestConnection = connectionType;
}

bool HttpLib::HttpRequest::addHeader(const std::string &nameOfHeader, const std::string &valueOfHeader)
{
	if (!nameOfHeader.empty() && !valueOfHeader.empty())
	{
		m_otherHeaders.push_back(nameOfHeader + ": " + valueOfHeader + "\r\n");
		return true;
	}

	return false;
}

// method for build request
// it must call after set methods
void HttpLib::HttpRequest::build()
{
	std::size_t someHdrsCount = 0;

	// configurations by default
	if (m_requestPath.size())
	{
		if (m_requestPath[0] != '/')
		{
			std::string tmp = m_requestPath;
			m_requestPath = "/" + tmp;
		}
	}
	else
	{
		m_requestPath = "/";
	}

    // build request
    // main header
    m_compiledRequest = m_requestMethod == Method::Get ? "GET " : m_requestMethod == Method::Post ? "POST " : "HEAD ";
    m_compiledRequest += m_requestPath + " " + (m_requestVersion == Version::Version1_0 ? "HTTP/1.0" : "HTTP/1.1") + "\r\n";

    m_compiledRequest += "Host: " + m_requestHost + "\r\n";

    if (!m_requestUserAgent.empty())
    {
        m_compiledRequest += "User-Agent: " + m_requestUserAgent + "\r\n";
    }

    if (m_requestConnection == ConnectionType::Close)
    {
        m_compiledRequest += "Connection: close\r\n";
    }
    else
    {
        m_compiledRequest += "Connection: keep-alive\r\n";
    }

    //
    if ((someHdrsCount = m_otherHeaders.size()))
    {
        for (std::size_t i = 0; i < someHdrsCount; ++i)
        {
            m_compiledRequest += m_otherHeaders[i];
        }
    }

    m_compiledRequest += "\r\n";
}

const HttpLib::HttpRequest::Method &HttpLib::HttpRequest::method() const
{
	return m_requestMethod;
}

const std::string &HttpLib::HttpRequest::relativePath() const
{
	return m_requestPath;
}

HttpLib::HttpRequest::Version HttpLib::HttpRequest::httpVersion() const
{
	return m_requestVersion;
}

const std::string &HttpLib::HttpRequest::host() const
{
	return m_requestHost;
}

const std::string &HttpLib::HttpRequest::userAgent() const
{
	return m_requestUserAgent;
}

const HttpLib::HttpRequest::ConnectionType & HttpLib::HttpRequest::connectionType() const
{
	return m_requestConnection;
}

const std::string &HttpLib::HttpRequest::compiledRequest() const
{
	return m_compiledRequest;
}

void HttpLib::HttpRequest::clear()
{
	m_requestConnection = ConnectionType::KeepAlive;
	m_requestVersion = Version::Version1_1;
	m_requestMethod = Method::Get;

	m_compiledRequest.clear();
	m_requestHost.clear();
	m_requestPath.clear();
	m_requestUserAgent.clear();
	m_otherHeaders.clear();
}
