#pragma once
#include <string>
#include <unordered_map>
#include "SerializableObject.h"
#include "Http1_1Packet.h"
#include "StringSerializer.h"

namespace Net::Http
{
	using HttpSessionStorage = std::unordered_map<std::wstring, Serializing::SerializableObject<Serializing::StringSerializer>>;
}
