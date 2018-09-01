#pragma once
#include <string>
#include <exception>
#include <tchar.h>
#include <Windows.h>

namespace Exceptions
{
	template<typename CharT>
	class BasicError : public std::exception
	{
		std::basic_string<CharT> data;
	protected:
		virtual std::basic_string<CharT> CodeToMessage(int errCode)
		{
			CharT buffer[256];
			if constexpr (std::is_same_v<CharT, TCHAR>)
			{
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errCode, 0, buffer, sizeof(buffer), NULL);
			}
			else if constexpr (std::is_same_v<CharT, char>)
			{
				FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errCode, 0, buffer, sizeof(buffer), NULL);
			}
			else if constexpr (std::is_same_v<CharT, char16_t>)
			{
				FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errCode, 0, buffer, sizeof(buffer), NULL);
			}
			else
			{
				static_assert(false, "CharT not support");
			}
			return buffer;
		}

	public:
		BasicError(int errCode)
		{
			data = CodeToMessage(errCode);
		}
		BasicError(const CharT* msg) : data(msg)
		{

		}

		BasicError(const std::basic_string<CharT> msg) : data(msg)
		{

		}

		const std::basic_string<CharT>& Message() const noexcept
		{
			return data;
		}
		const char* what() const override
		{
			return "Use Message() instead of what()";
		}

		virtual ~BasicError()
		{

		}
	};

	using Error = BasicError<TCHAR>;
}