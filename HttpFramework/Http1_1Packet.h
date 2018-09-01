#pragma once
#include "HttpHeaderCollection.h"
#include <string>

namespace Net::Http
{
	enum class EHttpDirection
	{
		Request,
		Response
	};

	class Http1_1Packet
	{
		std::string method;
		EHttpDirection direction;
		int statusCode;
		std::string statusText;
		std::string uri;
		std::string query;
		std::string version;
		HttpHeaderCollection headers;
		std::vector<std::byte> bodyBytes;
		std::vector<std::string> encodingChain;

	public:
		friend class Http1_1Server;

		Http1_1Packet();
		HttpHeaderCollection& GetHeaders()
		{
			return headers;
		}

		const HttpHeaderCollection& GetHeaders() const
		{
			return headers;
		}

		std::vector<std::byte>& GetBodyBytes()
		{
			return bodyBytes;
		}

		const std::vector<std::byte>& GetBodyBytes() const
		{
			return bodyBytes;
		}
		std::string& GetMethod() const;
		std::string& GetRequestUri() const;
		std::string& GetHttpVersion() const;
		std::string& GetQuery() const;
		std::string& GetCharacterSet() const;
		EHttpDirection& GetDirection() const;
		int& GetStatusCode() const;
		std::string& GetStatusText() const;
		virtual ~Http1_1Packet();
	};
}
