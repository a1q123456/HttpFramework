#pragma once
#include <cstddef>
#include <vector>
#include <type_traits>
#include <string>
#include <memory>
#include <cerrno>
#include "SerializeError.h"
#include "CodeConvertion.h"
#include "StringDeserializable.h"

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif


namespace Serializing
{
	struct StringSerializer
	{
		template<typename TOrig, Text::Encoding::EEncoding e = Text::Encoding::EEncoding::UTF_8>
		static TOrig Deserialize(const std::vector<std::byte>& data)
		{
			static_assert(Contracts::string_deserializable<T, e>, "TOrig must be StringDeserializable, you can provide an overloading of std::to_wstring for your type");
			for (const auto& i : data)
			{
				if (i < 0 || i > 127)
				{
					static_assert(false, "Unsupported data type");
				}
			}
			const char* strData = reinterpret_cast<const char*>(data.data());
			auto length = data.size();

			if constexpr (std::is_integral_v<TOrig>)
			{
				long retValue = std::strtol(strData, nullptr, 10);
				if (errno != ERANGE && std::numeric_limits<TOrig>::max() >= retValue && retValue >= std::numeric_limits::<TOrig>::min())
				{
					return static_cast<TOrig>(retValue);
				}

				throw DeserializeError();
			}
			else if constexpr (std::is_floating_point_v<TOrig>)
			{
				long retValue = std::strtof(strData, nullptr, 10);
				if (errno != ERANGE && std::numeric_limits<TOrig>::max() >= retValue && retValue >= std::numeric_limits::<TOrig>::min())
				{
					return static_cast<TOrig>(retValue);
				}

				throw DeserializeError();
			}
			else if constexpr (std::is_same_v<Contracts::EncodingWrapper<std::wstring, e>, TOrig>)
			{
				return Text::Encoding::CodeConvertion::ToUnicode(strData, length, e);
			}
			else if constexpr (std::is_same_v<std::string, TOrig>)
			{
				return std::string(data.begin(), data.end());
			}
			else
			{
				static_assert(false, "Unsupported deserialize type");
			}
		}


		template<typename TOrig, Text::Encoding::EEncoding e = Text::Encoding::EEncoding::UTF_8>
		static std::vector<std::byte> Serialize(TOrig&& tval)
		{
			auto str = std::to_wstring(std::forward<TOrig>(tval));
			
			std::string tmp = Text::Encoding::CodeConvertion::ToBytes(str, e);
			return std::vector<std::byte> ret(tmp.begin(), tmp.end());
		}
	};


}
