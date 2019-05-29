#pragma once

#include <string>

#include "../Models/Message.hpp"

namespace Irc::Parsers {

    bool parseMessage(const std::string& input, Irc::Models::Message& message);

}
