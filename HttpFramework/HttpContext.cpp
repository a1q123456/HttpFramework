#include "stdafx.h"
#include "HttpContext.h"

namespace Net::Http
{
	HttpContext::HttpContext(Http1_1Packet& request, Http1_1Packet& response):
		Request(request), Response(response)
	{

	}

	HttpContext::~HttpContext()
	{
	}
}

