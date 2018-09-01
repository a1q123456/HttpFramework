#pragma once
#include <string>
#include <vector>
#include "Serializing.h"
#include "SerializeError.h"

namespace Serializing
{
	template <typename TSerialzier>
	class SerializableObject
	{
		static_assert(Contracts::is_serializer<TSerialzier>, "TSerialzier must be a serialzer type");
		std::vector<std::byte> _underlying;
	public:
		SerializableObject(std::vector<std::byte> underlying) : _underlying(std::move(underlying))
		{
		}

		template<typename TOrig>
		TOrig Get()
		{
			if (TSerialzier::IsType<TOrig>())
			{
				TSerialzier::Deserialize<TOrig>(_underlying);
			}
			else
			{
				throw DeserializeError();
			}
		}

		template<typename TOrig>
		void Set(TOrig&& tval)
		{
			_underlying = TSerialzier::Serialize(tval);
		}

		virtual ~SerializableObject()
		{

		}
	};
}



