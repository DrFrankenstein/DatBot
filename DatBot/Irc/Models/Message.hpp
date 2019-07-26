#pragma once

#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace Irc::Models
{
struct Message
{
	using Tags    = std::unordered_map<std::string, std::string>;
	using Command = std::variant<std::uint16_t, std::string>;
	using Params  = std::vector<std::string>;

	Command command;
	Params params;
	std::string prefix;
	Tags tags;

	bool isNumeric() const;
	bool isCommand() const;
	bool is(std::uint16_t numeric) const;
	bool is(const std::string& command) const;

	std::string toString() const;
};

}

std::ostream& operator<<(std::ostream& out, const Irc::Models::Message& message);
