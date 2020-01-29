#pragma once

#include <string>
#include <unordered_map>

#include "Models/Channel.hpp"

namespace Irc
{
class Session;

class ChannelManager
{
	public:
	ChannelManager(Session& session);

	Models::Channel join(std::string name);

	private:
	Session& _session;
	std::unordered_map<std::string, Models::Channel> _channels;
};

}
