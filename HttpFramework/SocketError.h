#pragma once
#include <string>
#include <exception>
#include "Error.h"

namespace Net::Sockets
{
	template<typename CharT>
	class BasicSocketError : public Exceptions::BasicError<CharT>
	{
	public:
		BasicSocketError(int errCode) : Exceptions::BasicError<CharT>(errCode) {}
		BasicSocketError(CharT* err) : Exceptions::BasicError<CharT>(err) {}
		BasicSocketError(std::basic_string<CharT> err) : Exceptions::BasicError<CharT>(err) {}
	};

	using SocketError = BasicSocketError<TCHAR>;
}