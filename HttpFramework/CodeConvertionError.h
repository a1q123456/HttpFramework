#pragma once
#include "Error.h"

template <typename CharT>
class BasicCodeConvertionError : public Exceptions::BasicError<CharT>
{
public:
	BasicCodeConvertionError(int errCode) : Exceptions::BasicError<CharT>(errCode) {}
	BasicCodeConvertionError(CharT* err) : Exceptions::BasicError<CharT>(err) {}
	BasicCodeConvertionError(std::basic_string<CharT> err) : Exceptions::BasicError<CharT>(err) {}
};

using CodeConvertionError = BasicCodeConvertionError<TCHAR>;