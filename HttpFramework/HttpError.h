#pragma once
#include "Error.h"

template <typename CharT>
class BasicHttpError : public Exceptions::BasicError<CharT>
{
protected:
	virtual std::basic_string<CharT> CodeToMessage(int errCode) override
	{
		switch (errCode)
		{
			
		}
		return std::basic_string<CharT>;
	}

public:
	BasicCodeConvertionError(int errCode) : Exceptions::BasicError<CharT>(errCode) {}

};

using HttpError = BasicHttpError<TCHAR>;