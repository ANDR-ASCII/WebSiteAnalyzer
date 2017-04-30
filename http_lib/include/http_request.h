#pragma once

#include "stdlibs.h"

namespace HttpLib
{

    /**
    **
    ** by default:
    ** method: GET
    ** relative path: /
    ** version of protocol: HTTP/1.1
    ** connection type: keep-alive
    **
    ***/

	class HttpRequest
	{
	public:
		enum class Method { Get, Post, Head };
		enum class ConnectionType { KeepAlive, Close };
		enum class Version { V_1_0, V_1_1 };

		HttpRequest();
		HttpRequest(const HttpRequest &copyRequest);
		HttpRequest &operator=(const HttpRequest &copyRequest);

		bool operator==(const HttpRequest &cmpRequest);
		bool operator!=(const HttpRequest &cmpRequest);

		void setMethod(const Method method);
		void setRelativePath(const std::string &pathToRes);
		void setHttpVersion(Version httpVersion);
		void setHost(const std::string &host);
		void setUserAgent(const std::string &userAgent);
		void setConnectionType(const ConnectionType connectionType);

		bool addHeader(const std::string &nameOfHeader, const std::string &valueOfHeader);

		// before build new request
		// you must delete old request by calling this method
		void clear();

		/**
		**
		** method for build request
		** it must call after set methods
		** if method, relative path and version not specified, by default using:
        ** method: GET
        ** relative path: /
        ** version of protocol: HTTP/1.1
        **
        **/

		void build();

		// getters
		const Method& method() const;
		const std::string& relativePath() const;
		Version httpVersion() const;
		const std::string& host() const;
		const std::string& userAgent() const;
		const ConnectionType& connectionType() const;
		const std::string& compiledRequest() const;

	private:
		std::string m_compiledRequest;
		std::string m_requestPath;
		std::string m_requestHost;
		Version m_requestVersion;
		std::string m_requestUserAgent;
		Method m_requestMethod;
		ConnectionType m_requestConnection;
		std::vector<std::string> m_otherHeaders;
	};

}