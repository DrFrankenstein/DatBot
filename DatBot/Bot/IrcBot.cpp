#include "IrcBot.hpp"
#include "Config.hpp"
#include "../Net/AsioDevice.hpp"
#include "../Irc/Session.hpp"

#include <boost/range/algorithm/transform.hpp>
#include <exception>
#include <string>
#include <yaml-cpp/yaml.h>

using Net::AsioDevice;

using std::string;
using YAML::Node;

namespace Bot
{
IrcBot::IrcBot(AsioDevice& device, const string& nickname, const string& realname):
    _session(device, nickname, realname)
{
}

IrcBot::IrcBot(Net::AsioDevice& device, const Node& configroot):
    _session(device, "Unconfigured", "Unconfigured")
{
	readConfig(configroot);
}

void IrcBot::start()
{
	_session.start();
}

void IrcBot::readConfig(const Node& configroot)
{
	_session.nickname(Config::expectScalar<string>(configroot, "nick"));
	Config::getSequence<string>(configroot, "channels", back_inserter(_channels));
	_trigger = Config::defaultScalar<string>(configroot, "trigger", "!");
}
}
