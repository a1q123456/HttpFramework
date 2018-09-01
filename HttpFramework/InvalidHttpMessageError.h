#pragma once
#include "Error.h"

template <typename CharT>
class BasicInvalidHttpMessageError : public Exceptions::BasicError<CharT>
{
public:
	BasicInvalidHttpMessageError(int errCode) : Exceptions::BasicError<CharT>(errCode) {}
	BasicInvalidHttpMessageError(CharT* err) : Exceptions::BasicError<CharT>(err) {}
	BasicInvalidHttpMessageError(std::basic_string<CharT> err) : Exceptions::BasicError<CharT>(err) {}
	BasicInvalidHttpMessageError() {}
};

using InvalidHttpMessageError = BasicInvalidHttpMessageError<TCHAR>;