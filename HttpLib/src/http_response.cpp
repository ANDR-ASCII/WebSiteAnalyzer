#include "stdlibs.h"
#include "http_response.h"
#include "common.h"

HttpLib::HttpResponse::HttpResponse()
{
    m_serverResponseCode = UNSET;
}

HttpLib::HttpResponse::HttpResponse(const std::string &serverResponse)
{
    m_serverResponseCode = UNSET;
	setResponse(serverResponse);
}

void HttpLib::HttpResponse::setResponse(const std::string &serverResponse)
{
	std::string tempStrForHeader;
	auto endOfHeaders = serverResponse.find("\r\n\r\n");
	auto answerLength = serverResponse.size();

	// not found end of header
	if (endOfHeaders == std::string::npos)
	{
		m_invalidResponse = true;
		return;
	}

	for (decltype(endOfHeaders) i = 0; i < endOfHeaders; i++)
	{
		// add header in the vector
		if (serverResponse[i] == '\r' || i == (endOfHeaders - 1))
		{
			// in order not to lose the last character in the last response header
			if (i == (endOfHeaders - 1)) tempStrForHeader += serverResponse[i];

			if (((endOfHeaders - i) > 1 && serverResponse[i + 1] == '\n') || i == (endOfHeaders - 1))
			{
				structOfHeader header;
				auto lengthOfHeader = tempStrForHeader.size(), dblDot = tempStrForHeader.find(':');

				if (dblDot != std::string::npos)
				{
					for (std::string::size_type j = 0; tempStrForHeader[j] != ':'; j++) header.nameHeader += tempStrForHeader[j];
					for (auto j = dblDot + 1; j < lengthOfHeader; j++) header.valueHeader += tempStrForHeader[j];

					Common::cutSpaceChars(header.nameHeader);
					Common::cutSpaceChars(header.valueHeader);

					m_responseHeaders.push_back(header);
					tempStrForHeader.clear();
				}
				else
				{
					if (tempStrForHeader.find("HTTP") == std::string::npos)
					{
						// unavailable format of response
                        // not found colon character
                        // in the end of header
						m_invalidResponse = true;
						return;
					}
					else
					{
						for (decltype(lengthOfHeader) j = 0; j < lengthOfHeader; j++) header.valueHeader += tempStrForHeader[j];

                        // detects possible errors
                        determineResponseCode(header.valueHeader);
						m_responseHeaders.push_back(header);
						tempStrForHeader.clear();
					}
				}
			}
			else
			{
			    // unavailable format of response
			    // not found character of newline
			    // in the end of header
				m_invalidResponse = true;
				return;
			}
		}
		else
		{
			// reads header
			tempStrForHeader += serverResponse[i];
		}
	}

    // all headers of http response was read
    // endOfHeaders + 4 - label of start entity body(first character)

    std::string testEncoding;
    header("transfer-encoding", testEncoding);

    if(testEncoding.empty())
	{
	    for (auto i = endOfHeaders + 4; i < answerLength; i++)
        {
            m_entityBody += serverResponse[i];
        }
	}
	else if(testEncoding == "chunked")
    {
        // algorithm of read chunks of entity body
        std::string hexNumber, chunk;

        auto positionOfBeginHexNumber = endOfHeaders + 4;
        auto size = serverResponse.size();

        while(positionOfBeginHexNumber < size)
        {
            auto lblOfEndNumber = serverResponse.find("\r\n", positionOfBeginHexNumber);

            hexNumber.assign(serverResponse, positionOfBeginHexNumber, lblOfEndNumber);
            positionOfBeginHexNumber = lblOfEndNumber + 2;

            // for conversion to valid form for method stoul
            //hexNumber = "0x" + hexNumber;

            // why in code::blocks on windows 7 x64 with compiler GNU GCC 4.8.1
            // i don't use stoul??? Was not declared in this scope, but
            // string library was included
            auto num = std::strtoul(hexNumber.c_str(), 0, 0x10);
            if(!num)
            {
                break;
            }

            chunk.assign(serverResponse, positionOfBeginHexNumber, num);
            m_entityBody += chunk;

            positionOfBeginHexNumber += num + 2;
        }
    }
}

// headers of response
void HttpLib::HttpResponse::allHeaders(std::string &resultHeadersStr) const
{
	resultHeadersStr.clear();
	auto countHeaders = m_responseHeaders.size();

	for (decltype(countHeaders) i = 0; i < countHeaders; i++)
	{
		if (m_responseHeaders[i].valueHeader.find("HTTP") != std::string::npos)
		{
			resultHeadersStr += m_responseHeaders[i].valueHeader + "\r\n";
		}
		else
		{
			resultHeadersStr += m_responseHeaders[i].nameHeader + ": " + m_responseHeaders[i].valueHeader + "\r\n";
		}

		if ( (countHeaders - i) == 1)
		{
			resultHeadersStr += "\r\n";
		}
	}
}

bool HttpLib::HttpResponse::empty() const
{
    return m_entityBody.empty();
}

void HttpLib::HttpResponse::clear()
{
	m_entityBody.clear();
	m_responseHeaders.clear();
	m_serverResponseCode = UNSET;
	m_invalidResponse = false;
}

int HttpLib::HttpResponse::responseCode() const
{
	return m_serverResponseCode;
}

const std::string & HttpLib::HttpResponse::entityBody() const
{
	return m_entityBody;
}

void HttpLib::HttpResponse::header(const std::string &header, std::string &valueOfHeader) const
{
    valueOfHeader.clear();

	for (const auto &hdr : m_responseHeaders)
    {
        if (Common::strToLower(hdr.nameHeader) == 
			Common::strToLower(header))
        {
           valueOfHeader = hdr.valueHeader;
           break;
        }
    }
}

bool HttpLib::HttpResponse::isValid() const { return !m_invalidResponse; }


/** information codes **/

bool HttpLib::HttpResponse::is_100_Continue() const
{
    return m_serverResponseCode == ResponseCode::Continue100;
}

bool HttpLib::HttpResponse::is_101_SwitchingProtocols() const
{
    return m_serverResponseCode == ResponseCode::SwitchingProtocol101;
}

/** successful codes **/

bool HttpLib::HttpResponse::is_200_Ok() const
{
    return m_serverResponseCode == ResponseCode::Ok200;
}

bool HttpLib::HttpResponse::is_201_Created() const
{
    return m_serverResponseCode == ResponseCode::Created201;
}

bool HttpLib::HttpResponse::is_202_Accepted() const
{
    return m_serverResponseCode == ResponseCode::Accepted202;
}

bool HttpLib::HttpResponse::is_203_NonAuthInformation() const
{
    return m_serverResponseCode == ResponseCode::NonAuthoritativeInformation203;
}

bool HttpLib::HttpResponse::is_204_NoContent() const
{
    return m_serverResponseCode == ResponseCode::NoContent204;
}

bool HttpLib::HttpResponse::is_205_ResetContent() const
{
    return m_serverResponseCode == ResponseCode::ResetContent205;
}

bool HttpLib::HttpResponse::is_206_PartialContent() const
{
    return m_serverResponseCode == ResponseCode::PartialContent206;
}

/** Codes redirection **/

bool HttpLib::HttpResponse::is_300_MultipleChoices() const
{
    return m_serverResponseCode == ResponseCode::MultipleChoices300;
}

bool HttpLib::HttpResponse::is_301_MovedPermanently() const
{
    return m_serverResponseCode == ResponseCode::MovedPermanently301;
}

bool HttpLib::HttpResponse::is_302_MovedTemporarily() const
{
    return m_serverResponseCode == ResponseCode::MovedTemporarily302;
}

bool HttpLib::HttpResponse::is_303_SeeOther() const
{
    return m_serverResponseCode == ResponseCode::SeeOther303;
}

bool HttpLib::HttpResponse::is_304_NotModified() const
{
    return m_serverResponseCode == ResponseCode::NotModified304;
}

bool HttpLib::HttpResponse::is_305_UseProxy() const
{
    return m_serverResponseCode == ResponseCode::UseProxy305;
}

/** Client errors **/

bool HttpLib::HttpResponse::is_400_BadRequest() const
{
    return m_serverResponseCode == ResponseCode::BadRequest400;
    }

bool HttpLib::HttpResponse::is_401_Unauthorized() const
{
    return m_serverResponseCode == ResponseCode::Unauthorized401;
}

bool HttpLib::HttpResponse::is_402_PaymentRequired() const
{
    return m_serverResponseCode == ResponseCode::PaymentRequired402;
}

bool HttpLib::HttpResponse::is_403_Forbidden() const
{
    return m_serverResponseCode == ResponseCode::Forbidden403;
}

bool HttpLib::HttpResponse::is_404_NotFound() const
{
    return m_serverResponseCode == ResponseCode::NotFound404;
}

bool HttpLib::HttpResponse::is_405_MethodNotAllowed() const
{
    return m_serverResponseCode == ResponseCode::MethodNotAllowed405;
}

bool HttpLib::HttpResponse::is_406_NotAcceptable() const
{
    return m_serverResponseCode == ResponseCode::NonAcceptable406;
}

bool HttpLib::HttpResponse::is_407_ProxyAuthenticationRequired() const
{
    return m_serverResponseCode == ResponseCode::ProxyAuthenticationRequired407;
}

bool HttpLib::HttpResponse::is_408_RequestTimeout() const
{
    return m_serverResponseCode == ResponseCode::RequestTimeout408;
}

bool HttpLib::HttpResponse::is_409_Conflict() const
{
    return m_serverResponseCode == ResponseCode::Conflict409;
}

bool HttpLib::HttpResponse::is_410_Gone() const
{
    return m_serverResponseCode == ResponseCode::Gone410;
}

bool HttpLib::HttpResponse::is_411_LengthRequired() const
{
    return m_serverResponseCode == ResponseCode::LengthRequired411;
}

bool HttpLib::HttpResponse::is_412_PreconditionFailed() const
{
    return m_serverResponseCode == ResponseCode::PreconditionFailed412;
}

bool HttpLib::HttpResponse::is_413_RequestEntityTooLarge() const
{
    return m_serverResponseCode == ResponseCode::RequestEntityTooLarge413;
}

bool HttpLib::HttpResponse::is_414_RequestURITooLong() const
{
    return m_serverResponseCode == ResponseCode::RequestUriTooLong414;
}

bool HttpLib::HttpResponse::is_415_UnsupportedMediaType() const
{
    return m_serverResponseCode == ResponseCode::UnsupportedMediaType415;
}

/** error codes of server **/

bool HttpLib::HttpResponse::is_500_InternalServerError() const
{
    return m_serverResponseCode == ResponseCode::InternalServerError500;
}

bool HttpLib::HttpResponse::is_501_NotImplemented() const
{
    return m_serverResponseCode == ResponseCode::NotImplemented501;
}

bool HttpLib::HttpResponse::is_502_BadGateway() const
{
    return m_serverResponseCode == ResponseCode::BadGateway502;
}

bool HttpLib::HttpResponse::is_503_ServiceUnavailable() const
{
    return m_serverResponseCode == ResponseCode::ServiceUnavailable503;
}

bool HttpLib::HttpResponse::is_504_GatewayTimeout() const
{
    return m_serverResponseCode == ResponseCode::GatewayTimeout504;
}

bool HttpLib::HttpResponse::is_505_HttpVersionNotSupported() const
{
    return m_serverResponseCode == ResponseCode::HttpVersionNotSupported505;
}

// sets flags of errors if it detected
void HttpLib::HttpResponse::determineResponseCode(const std::string &checkStr)
{
    std::string temporary = Common::strToLower(checkStr);

    // 100 Continue
    if (temporary.find("100") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::Continue100;
		return;
	}

    // 101 Switching Protocols
    if (temporary.find("101") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::SwitchingProtocol101;
		return;
	}

    // 200 OK
    if (temporary.find("200") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::Ok200;
		return;
	}

    // 201 Created
    if (temporary.find("201") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::Created201;
		return;
	}

    // 202 Accepted
    if (temporary.find("202") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::Accepted202;
		return;
	}

    // 203 Non-Authoritative Information
    if (temporary.find("203") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::NonAuthoritativeInformation203;
		return;
	}

    // 204 No Content
    if (temporary.find("204") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::NoContent204;
		return;
	}

    // 205 Reset Content
    if (temporary.find("205") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::ResetContent205;
		return;
	}

	// 206 Partial Content
	if (temporary.find("206") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::PartialContent206;
		return;
	}

    // 300 Multiple Choices
    if (temporary.find("300") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::MultipleChoices300;
		return;
	}

    // 301 Moved Permanently
	if (temporary.find("301") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::MovedPermanently301;
		return;
	}

	// 302 Moved Temporarily
    if (temporary.find("302") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::MovedTemporarily302;
		return;
    }

    // 303 See Other
    if (temporary.find("303") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::SeeOther303;
		return;
    }

    // 304 Not Modified
    if (temporary.find("304") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::NotModified304;
		return;
    }

    // 305 Use Proxy
    if (temporary.find("305") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::UseProxy305;
		return;
    }

    // 400 Bad Request
	if (temporary.find("400") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::BadRequest400;
		return;
    }

    // 401 Unauthorized
	if (temporary.find("401") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::Unauthorized401;
		return;
	}

	// 402 Payment Required
	if (temporary.find("402") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::PaymentRequired402;
		return;
	}

    // 403 Forbidden
	if (temporary.find("403") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::Forbidden403;
		return;
	}

	// 404 Not Found
	if (temporary.find("404") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::NotFound404;
		return;
	}

    // 405 Method Not Allowed
	if (temporary.find("405") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::MethodNotAllowed405;
		return;
	}

	// 406 Not Acceptable
	if (temporary.find("406") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::NonAcceptable406;
		return;
	}

	// 407 Proxy Authentication Required
	if (temporary.find("407") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::ProxyAuthenticationRequired407;
		return;
	}

	// 408 Request Timeout
	if (temporary.find("408") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::RequestTimeout408;
		return;
	}

	// 409 Conflict
	if (temporary.find("409") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::Conflict409;
		return;
	}

	// 410 Gone
	if (temporary.find("410") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::Gone410;
		return;
	}

	// 411 Length Required
	if (temporary.find("411") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::LengthRequired411;
		return;
	}

	// 412 Precondition Failed
	if (temporary.find("412") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::PreconditionFailed412;
		return;
	}

	// 413 Request Entity Too Large
	if (temporary.find("413") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::RequestEntityTooLarge413;
		return;
	}

	// 414 Request-URI Too Long
	if (temporary.find("414") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::RequestUriTooLong414;
		return;
	}

	// 415 Unsupported Media Type
	if (temporary.find("415") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::UnsupportedMediaType415;
		return;
	}

	// 500 Internal Server Error
	if (temporary.find("500") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::InternalServerError500;
		return;
	}

	// 501 Not Implemented
	if (temporary.find("501") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::NotImplemented501;
		return;
	}

    // 502 Bad Gateway
	if (temporary.find("502") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::BadGateway502;
		return;
	}

    // 503 Service Unavailable
	if (temporary.find("503") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::ServiceUnavailable503;
		return;
	}

    // 504 Gateway Timeout
	if (temporary.find("504") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::GatewayTimeout504;
		return;
	}

    // 505 HTTP Version Not Supported
	if (temporary.find("505") != std::string::npos)
	{
		m_serverResponseCode = ResponseCode::HttpVersionNotSupported505;
		return;
	}
}
