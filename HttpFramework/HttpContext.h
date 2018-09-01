#pragma once
#include "Http1_1Packet.h"
#include "HttpCookieStorage.h"
#include "HttpSessionStorage.h"

namespace Net::Http
{
	class HttpContext
	{

	public:
		Http1_1Packet& Request;
		Http1_1Packet& Response;
		HttpSessionStorage Sessions;
		HttpCookieStorage Cookies;

		// TODO

		HttpContext(Http1_1Packet& request, Http1_1Packet& response);
		virtual ~HttpContext();
	};

}

