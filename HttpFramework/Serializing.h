#pragma once
#include <vector>
#include <cstddef>
#include <type_traits>

namespace Contracts
{
	template <typename T, typename TS, typename TOrig, typename = void>
	struct is_serializer : std::false_type
	{
	};

	template <typename T, typename TS, typename TOrig, typename std::enable_if<
		// TOrig T::Deserialize(std::vector<std::byte>> msg);
		std::is_same_v<TOrig, decltype(T::Deserialize<TOrig>(std::declval<std::vector<std::byte>>()))> &&
		// std::vector<std::byte> T::Serialize(TOrig obj);
		std::is_same_v<std::vector<std::byte>, decltype(T::Serialize(std::declval<TOrig>()))>::value
		>>
		struct is_serializer : std::true_type
	{

	};
}

