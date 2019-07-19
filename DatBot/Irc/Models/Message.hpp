#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>

namespace Irc::Models
{
struct Message
{
	std::unordered_map<std::string, std::string> tags;
	std::string prefix;
	std::variant<std::uint16_t, std::string> command;
	std::list<std::string> params;
};

}
