#include "IrcBot.hpp"
#include "../Net/AsioDevice.hpp"
#include "../Irc/Session.hpp"

#include <boost/range/algorithm/transform.hpp>
#include <exception>
#include <string>
#include <yaml-cpp/yaml.h>

using Net::AsioDevice;

using boost::transform;
using std::runtime_error, std::string;
using YAML::Node;

namespace Bot
{
IrcBot::IrcBot(AsioDevice& device, const string& nickname, const string& realname):
    _session(device, nickname, realname)
{
	device.connect();
}
IrcBot::IrcBot(Net::AsioDevice& device, const Node& configroot):
    _session(device, "Unconfigured", "Unconfigured")
{
	readConfig(configroot);
}

void IrcBot::readConfig(const Node& configroot)
{
	auto nick = configroot["nick"];
	if (!nick || !nick.IsScalar())
		throw runtime_error { "config: 'nick' is required and must be a string" };

	_session.nickname(nick.as<string>());

	auto channels = configroot["channels"];
	if (channels)
	{
		if (!channels.IsSequence())
			throw runtime_error { "config: 'channels' is malformed" };

		_channels.reserve(channels.size());
		transform(channels, back_inserter(_channels), [](auto channelnode) { return channelnode.as<string>(); });
	}

	auto trigger = configroot["trigger"];
	if (trigger)
		_trigger = trigger.as<string>();
	else
		_trigger = "!";
}
}
