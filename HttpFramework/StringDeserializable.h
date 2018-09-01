#pragma once
#include <type_traits>
#include <string>
#include "CodeConvertion.h"

namespace Contracts
{
	template<typename TString, Text::Encoding::EEncoding e>
	struct EncodingWrapper
	{

	};

	template<typename T, Text::Encoding::EEncoding e = Text::Encoding::EEncoding::UTF_8, typename = void>
	struct string_deserializable : std::false_type {};

	template<typename T, Text::Encoding::EEncoding e = Text::Encoding::EEncoding::UTF_8, typename = std::enable_if<
		std::is_integral_v<T> ||
		std::is_floating_point_v<T> ||
		std::is_same_v<EncodingWrapper<std::wstring, e>, T> ||
		std::is_same_v<std::string, T>
	>::value>
	struct string_deserializable : std::true_type {};
}