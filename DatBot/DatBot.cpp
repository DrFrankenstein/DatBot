#include "Irc/Session.hpp"
#include "Net/AsioDevice.hpp"

#include <algorithm>
#include <boost/asio.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <exception>
#include <filesystem>
#include <iterator>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace asio = boost::asio;
using boost::transform;
using Irc::Session;
using std::back_inserter, std::runtime_error, std::string, std::vector;
using YAML::Node;

namespace Bot
{
class IrcBot
{
	public:
	IrcBot(Net::AsioDevice& device, const string& nickname, const string& realname):
	    _session(device, nickname, realname)
	{
		device.connect();
	}

	IrcBot(Net::AsioDevice& device, const Node& configroot):
	    _session(device, "Unconfigured", "Unconfigured")
	{
		readConfig(configroot);
	}

	private:
	void readConfig(const Node& configroot)
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

			transform(channels, back_inserter(_channels), [](auto channelnode) { return channelnode.as<string>(); });
		}

		auto trigger = configroot["trigger"];
		if (trigger)
			_trigger = trigger.as<string>();
		else
			_trigger = "!";
	}

	Session _session;
	string _trigger;
	vector<string> _channels;
};
}

int main()
{
	Node config = YAML::LoadFile("datbot.conf");
	std::cout << "config:" << config << std::endl;

	asio::io_context io;
	Net::AsioDevice tcpreader(io, "irc.snoonet.org", 6667);
	Bot::IrcBot bot(tcpreader, config);

	io.run();
}
