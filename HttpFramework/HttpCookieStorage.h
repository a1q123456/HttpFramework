#pragma once

namespace Net::Http
{
	using HttpCookieStorage = std::unordered_map<std::wstring, Serializing::SerializableObject<Serializing::StringSerializer>>;
}

