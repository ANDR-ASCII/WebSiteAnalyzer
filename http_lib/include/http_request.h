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
		const Method& getMethod() const;
		const std::string& getRelativePath() const;
		Version getHttpVersion() const;
		const std::string& getHost() const;
		const std::string& getUserAgent() const;
		const ConnectionType& getConnectionType() const;
		const std::string& getResRequest() const;

	private:

		std::string m_resRequest;
		std::string m_reqPath;
		std::string m_reqHost;
		Version reqVersion;
		std::string m_reqUserAgent;
		Method m_reqMethod;
		ConnectionType m_reqConnection;
		std::vector<std::string> m_otherHeaders;
	};

}