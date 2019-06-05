#pragma once

#include <string>

#include "../Models/Message.hpp"

namespace Irc::Parsers {

    Models::Message tryParseMessage(const std::string& input);
    
    bool parseMessage(const std::string& input, Irc::Models::Message& message);

}
