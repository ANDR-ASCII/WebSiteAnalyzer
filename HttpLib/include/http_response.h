#pragma once

#include "stdlibs.h"

namespace HttpLib
{
	struct structOfHeader
	{
		std::string nameHeader;
		std::string valueHeader;
	};

    /**
    **
    ** This object create and edit may only HttpConnection objects.
    ** Also anyone can create this object using copy constructor.
    ** But cannot change, if only using the operator =.
    ** This features allows to keep an object in an other object if you need to store data.
    **
    **/

	class HttpResponse
	{
    public:

        enum ResponseCode
        {
              Continue100 = 100
            , SwitchingProtocol101
            //=================================
            , Ok200 = 200
            , Created201
            , Accepted202
            , NonAuthoritativeInformation203
            , NoContent204
            , ResetContent205
            , PartialContent206
            //=================================
            , MultipleChoices300 = 300
            , MovedPermanently301
            , MovedTemporarily302
            , SeeOther303
            , NotModified304
            , UseProxy305
            //=================================
            , BadRequest400 = 400
            , Unauthorized401
            , PaymentRequired402
            , Forbidden403
            , NotFound404
            , MethodNotAllowed405
            , NonAcceptable406
            , ProxyAuthenticationRequired407
            , RequestTimeout408
            , Conflict409
            , Gone410
            , LengthRequired411
            , PreconditionFailed412
            , RequestEntityTooLarge413
            , RequestUriTooLong414
            , UnsupportedMediaType415
            //=================================
            , InternalServerError500 = 500
            , NotImplemented501
            , BadGateway502
            , ServiceUnavailable503
            , GatewayTimeout504
            , HttpVersionNotSupported505
        };

    private:

        const int UNSET = -1; // for flag responseOfServer_

		std::vector<structOfHeader> m_responseHeaders;	// headers of response of server
		std::string m_entityBody;					    // entity body obtained from response of server

		bool m_invalidResponse;						    // flag of validation of response
        int m_serverResponseCode;

        // the class whose objects can create objects of this type
        friend class HttpConnection;

        HttpResponse();
		HttpResponse(const std::string& serverResponse); // in moment of create object must to initialize of response of server
		void setResponse(const std::string& serverResponse);

		void readChunks(const std::string& entityBody);

		void determineResponseCode(const std::string& checkStr);

	public:

		bool isValid()                            const;

		/** is_ERRNUM_Transcription() **/

        /** information codes **/
        bool is_100_Continue()                    const;
        bool is_101_SwitchingProtocols()          const;

        /** Successful codes **/
        bool is_200_Ok()                          const;
        bool is_201_Created()                     const;
        bool is_202_Accepted()                    const;
        bool is_203_NonAuthInformation()          const;
        bool is_204_NoContent()                   const;
        bool is_205_ResetContent()                const;
        bool is_206_PartialContent()              const;

        /** Codes redirection **/
        bool is_300_MultipleChoices()             const;
        bool is_301_MovedPermanently()            const;
        bool is_302_MovedTemporarily()            const;
        bool is_303_SeeOther()                    const;
        bool is_304_NotModified()                 const;
        bool is_305_UseProxy()                    const;

        /** Client errors **/
        bool is_400_BadRequest()                  const;
        bool is_401_Unauthorized()                const;
        bool is_402_PaymentRequired()             const;
        bool is_403_Forbidden()                   const;
        bool is_404_NotFound()                    const;
        bool is_405_MethodNotAllowed()            const;
        bool is_406_NotAcceptable()               const;
        bool is_407_ProxyAuthenticationRequired() const;
        bool is_408_RequestTimeout()              const;
        bool is_409_Conflict()                    const;
        bool is_410_Gone()                        const;
        bool is_411_LengthRequired()              const;
        bool is_412_PreconditionFailed()          const;
        bool is_413_RequestEntityTooLarge()       const;
        bool is_414_RequestURITooLong()           const;
        bool is_415_UnsupportedMediaType()        const;

        /** Error codes of server **/
        bool is_500_InternalServerError()         const;
        bool is_501_NotImplemented()              const;
        bool is_502_BadGateway()                  const;
        bool is_503_ServiceUnavailable()          const;
        bool is_504_GatewayTimeout()              const;
        bool is_505_HttpVersionNotSupported()     const;


		bool empty() const;
		void clear();

		/** ****************** getters ****************** **/

		int responseCode() const;
		const std::string &entityBody() const;

		// get all headers as string object like returns HEAD method
		void allHeaders(std::string &resultHeadersStr) const;

		// get specified header
		void header(const std::string &header, std::string &valueOfHeader) const;
	};

}