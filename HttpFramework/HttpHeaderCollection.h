#pragma once
#include <vector>
#include <unordered_map>

namespace Net::Http
{
	using HttpHeaderCollection = std::unordered_map<std::string, std::vector<std::string>>;
}
