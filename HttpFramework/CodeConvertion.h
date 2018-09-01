#pragma once

#include <memory>
#include "TextEncoding.h"
#include <Windows.h>
#include <string>
#include <exception>
#include "CodeConvertionError.h"

namespace Text::Encoding
{

	class CodeConvertion
	{
	public:
		enum class ECompositeCheck
		{
			DefaultChar = WC_DEFAULTCHAR,
			DiscardDns = WC_DISCARDNS,
			SepChars = WC_SEPCHARS,
			None = 0
		};

		static std::wstring ToUnicode(const char* str, const int& strLen, const EEncoding& from, bool failInvalidChar = false, bool composite = false, bool useDosGlyphs = false)
		{
			DWORD flags = 0;
			if (composite)
			{
				flags |= MB_COMPOSITE;
			}
			else
			{
				flags |= MB_PRECOMPOSED;
			}
			if (failInvalidChar)
			{
				flags |= MB_ERR_INVALID_CHARS;
			}
			if (useDosGlyphs)
			{
				flags |= MB_USEGLYPHCHARS;
			}

			int len = MultiByteToWideChar(static_cast<UINT>(from), flags, str, strLen, NULL, 0);
			if (len == 0)
			{
				throw CodeConvertionError(len);
			}
			auto ret = std::wstring(len, 0);
			MultiByteToWideChar(static_cast<UINT>(from), flags, str, strLen, ret.data(), len);
			return ret;
		}

		static std::string ToBytes(const wchar_t* str, const int& strLen, const EEncoding& to, bool failInvalidChar = false, ECompositeCheck compositeCheck = ECompositeCheck::SepChars, bool noBestFitChars = true)
		{
			DWORD flags = static_cast<DWORD>(compositeCheck);
			if (failInvalidChar)
			{
				flags |= WC_ERR_INVALID_CHARS;
			}
			if (noBestFitChars)
			{
				flags |= WC_NO_BEST_FIT_CHARS;
			}

			int len = WideCharToMultiByte(static_cast<UINT>(to), flags, str, strLen, NULL, 0, NULL, NULL);
			if (len == 0)
			{
				throw CodeConvertionError(len);
			}

			auto ret = std::string(len, 0);
			WideCharToMultiByte(static_cast<UINT>(to), flags, str, strLen, ret.data(), len, NULL, NULL);
			return ret;
		}

		static std::string ToBytes(const std::wstring& str, const EEncoding& to)
		{
			return ToBytes(str.c_str(), str.length(), to);
		}

		static std::wstring ToUnicode(const std::string& str, const EEncoding& to)
		{
			return ToUnicode(str.c_str(), str.length(), to);
		}
		
		template <std::size_t strLen>
		static std::string ToBytes(wchar_t (&str)[strLen], const EEncoding& to)
		{
			return ToBytes(str, strLen, to);
		}

		template <std::size_t strLen>
		static 	std::wstring ToUnicode(char (&str)[strLen], const EEncoding& to)
		{
			return ToUnicode(str, strLen, to);
		}
	};
}
