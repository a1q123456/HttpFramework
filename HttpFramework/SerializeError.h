#pragma once
#include "Error.h"

template <typename CharT>
class BasicDeserializeError : public Exceptions::BasicError<CharT>
{
public:
	BasicDeserializeError(std::basic_string<CharT> err) : Exceptions::BasicError<CharT>(err) {}
};

using DeserializeError = BasicDeserializeError<TCHAR>;


template <typename CharT>
class BasicSerializeError : public Exceptions::BasicError<CharT>
{
public:
	BasicSerializeError(std::basic_string<CharT> err) : Exceptions::BasicError<CharT>(err) {}
};

using SerializeError = BasicSerializeError<TCHAR>;