#pragma once

#include "stdlibs.h"
#include "http_request.h"
#include "http_response.h"

#define DEFAULT_TIMEOUT_MS 10000

namespace HttpLib
{
    /**

        Errors of function connect()

        ETIMEDOUT - is returned if on segment SYN no response by server

        ECONNREFUSED - is returned if server on segment SYN answered segment RST
                    This means server node does not contain any process related to listening on specified port.

        ENETUNREACH or EHOSTUNREACH - to the client in response to the SYN segment received ICMP message that unreachable
                                    the recipient from any intermediate routers.
                                    TCP tries send SYN segment three times.
                                    After attempts returns one from above errors.

        ENETUNREACH - is deprecated

    **/

    class HttpErrorException
    {
        std::string err_;
    public:
        HttpErrorException(const std::string &errWhat) : err_(errWhat) {}
        HttpErrorException(const char *errWhat) : err_(errWhat) {}

        virtual const char *what() const
        {
            return err_.c_str();
        }
    };

	class HttpConnection
	{

        #if PLATFORM == PLATFORM_WINDOWS

		WSADATA libVer_;

		// initialize WinSock2
		void init();

		#endif

		SOCKET mainSock_;
		sockaddr_in servAddr_;
		hostent *host_;


		bool connectionEstablished_; // flag of successful connection

        // Caching site name to prevent redundant queries to DNS server
		std::string lastServerName;

		// close opened socket
		void closeConnection();

        // object of response of HTTP server
		HttpResponse response_;

		// structure of file descriptors for test
		// our socket on ready able to read
		fd_set testSock;
		long mstimeout;

	public:

		HttpConnection();
		// connection establish with server which have needed resource
		HttpConnection(const std::string &server);
		~HttpConnection();

        /** ****************** interface ************************* **/

		void setConnection(const std::string &server);
        void setTimeOut(long milliseconds);
        long getTimeOut() const;

		const HttpResponse *openUrl(const HttpRequest &request);
        // send request on server, returns response
		const HttpResponse *sendRequest(const HttpRequest &request);
	};
}