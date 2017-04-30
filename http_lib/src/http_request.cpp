#include "stdlibs.h"
#include "http_request.h"
#include "tag_parser.h"
#include "url.h"

HttpLib::HttpRequest::HttpRequest()
    : reqVersion(Version::V_1_1)
    , m_reqMethod(Method::Get)
    , m_reqConnection(ConnectionType::KeepAlive)
{
}

HttpLib::HttpRequest::HttpRequest(const HttpRequest &copyRequest)
{
	m_resRequest = copyRequest.m_resRequest;
	m_reqConnection = copyRequest.m_reqConnection;
	m_reqHost = copyRequest.m_reqHost;
	m_reqMethod = copyRequest.m_reqMethod;
	m_reqPath = copyRequest.m_reqPath;
	m_reqUserAgent = copyRequest.m_reqUserAgent;
	reqVersion = copyRequest.reqVersion;
}

HttpLib::HttpRequest &HttpLib::HttpRequest::operator=(const HttpRequest &copyRequest)
{
	if (*this == copyRequest)
		return *this;

	m_resRequest = copyRequest.m_resRequest;
	m_reqConnection = copyRequest.m_reqConnection;
	m_reqHost = copyRequest.m_reqHost;
	m_reqMethod = copyRequest.m_reqMethod;
	m_reqPath = copyRequest.m_reqPath;
	m_reqUserAgent = copyRequest.m_reqUserAgent;
	reqVersion = copyRequest.reqVersion;

	return *this;
}

bool HttpLib::HttpRequest::operator==(const HttpRequest &cmpRequest)
{
	if (m_resRequest == cmpRequest.m_resRequest &&
		m_reqConnection == cmpRequest.m_reqConnection &&
		m_reqHost == cmpRequest.m_reqHost &&
		m_reqMethod == cmpRequest.m_reqMethod &&
		m_reqPath == cmpRequest.m_reqPath &&
		m_reqUserAgent == cmpRequest.m_reqUserAgent &&
		reqVersion == cmpRequest.reqVersion)

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
	m_reqMethod = method;
}

void HttpLib::HttpRequest::setRelativePath(const std::string &pathToRes)
{
	m_reqPath = pathToRes;
}

void HttpLib::HttpRequest::setHttpVersion(Version httpVersion)
{
	reqVersion = httpVersion;
}

void HttpLib::HttpRequest::setHost(const std::string &host)
{
	m_reqHost = host;
}

void HttpLib::HttpRequest::setUserAgent(const std::string &userAgent)
{
	m_reqUserAgent = userAgent;
}

void HttpLib::HttpRequest::setConnectionType(const ConnectionType connectionType)
{
	m_reqConnection = connectionType;
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
	if (m_reqPath.size())
	{
		if (m_reqPath[0] != '/')
		{
			std::string tmp = m_reqPath;
			m_reqPath = "/" + tmp;
		}
	}
	else
	{
		m_reqPath = "/";
	}

    // build request
    // main header
    m_resRequest = m_reqMethod == Method::Get ? "GET " : m_reqMethod == Method::Post ? "POST " : "HEAD ";
    m_resRequest += m_reqPath + " " + (reqVersion == Version::V_1_0 ? "HTTP/1.0" : "HTTP/1.1") + "\r\n";

    m_resRequest += "Host: " + m_reqHost + "\r\n";

    if (!m_reqUserAgent.empty())
    {
        m_resRequest += "User-Agent: " + m_reqUserAgent + "\r\n";
    }

    if (m_reqConnection == ConnectionType::Close)
    {
        m_resRequest += "Connection: close\r\n";
    }
    else
    {
        m_resRequest += "Connection: keep-alive\r\n";
    }

    //
    if ((someHdrsCount = m_otherHeaders.size()))
    {
        for (std::size_t i = 0; i < someHdrsCount; ++i)
        {
            m_resRequest += m_otherHeaders[i];
        }
    }

    m_resRequest += "\r\n";
}

const HttpLib::HttpRequest::Method &HttpLib::HttpRequest::getMethod() const
{
	return m_reqMethod;
}

const std::string &HttpLib::HttpRequest::getRelativePath() const
{
	return m_reqPath;
}

HttpLib::HttpRequest::Version HttpLib::HttpRequest::getHttpVersion() const
{
	return reqVersion;
}

const std::string &HttpLib::HttpRequest::getHost() const
{
	return m_reqHost;
}

const std::string &HttpLib::HttpRequest::getUserAgent() const
{
	return m_reqUserAgent;
}

const HttpLib::HttpRequest::ConnectionType & HttpLib::HttpRequest::getConnectionType() const
{
	return m_reqConnection;
}

const std::string &HttpLib::HttpRequest::getResRequest() const
{
	return m_resRequest;
}

void HttpLib::HttpRequest::clear()
{
	m_reqConnection = ConnectionType::KeepAlive;
	reqVersion = Version::V_1_1;
	m_reqMethod = Method::Get;

	m_resRequest.clear();
	m_reqHost.clear();
	m_reqPath.clear();
	m_reqUserAgent.clear();
	m_otherHeaders.clear();
}
