#pragma once

#include "../Models/Message.hpp"

#include <string>

namespace Irc::Parsers
{
Models::Message tryParseMessage(const std::string& input);

bool parseMessage(const std::string& input, Irc::Models::Message& message);

}
