#pragma once

#include <string>

namespace Irc::Models
{

struct PartEvent
{
	std::string channel;
	std::string reason;
};

}
