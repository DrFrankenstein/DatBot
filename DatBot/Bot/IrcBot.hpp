#include "../Net/AsioDevice.hpp"
#include "../Irc/Session.hpp"

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace Bot
{
class IrcBot
{
	public:
	IrcBot(Net::AsioDevice& device, const std::string& nickname, const std::string& realname);
	IrcBot(Net::AsioDevice& device, const YAML::Node& configroot);

	private:
	void readConfig(const YAML::Node& configroot);

	Irc::Session _session;
	std::string _trigger;
	std::vector<std::string> _channels;
};
}
