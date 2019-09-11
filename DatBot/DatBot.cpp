#include "Net/AsioDevice.hpp"
#include "Bot/IrcBot.hpp"

#include <boost/asio.hpp>
#include <yaml-cpp/yaml.h>

namespace asio = boost::asio;
using YAML::Node;

int main()
{
	Node config = YAML::LoadFile("datbot.conf");
	std::cout << "config:" << config << std::endl;

	asio::io_context io;
	Net::AsioDevice tcpreader(io, "irc.snoonet.org", 6667);
	Bot::IrcBot bot(tcpreader, config);

	io.run();
}
