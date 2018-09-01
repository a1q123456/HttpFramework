#pragma once
#include <type_traits>
#include <string>

namespace Contracts
{
	
	template<typename T, typename = void>
	struct string_serializable : std::false_type {};

	template<typename T, typename = std::enable_if<
		// std::to_wstring(tobj)
		std::is_same_v<decltype(std::to_wstring(std::declval<T>())), std::wstring>
	>::value>
		struct string_serializable : std::true_type {};
}