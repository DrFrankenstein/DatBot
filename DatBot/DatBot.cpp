#include "Bot/IrcBot.hpp"
#include "Net/AsioDevice.hpp"

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>

namespace asio = boost::asio;
namespace po   = boost::program_options;
using std::cout, std::endl, std::exit, std::string;
using YAML::Node;

po::variables_map parseOptions(int argc, char* argv[])
{
	po::options_description mainOptions { "Main options" };
	mainOptions.add_options()("help,?", "print this help")("config-file,f", po::value<string>()->default_value("datbot.conf"), "set config file path");

	po::variables_map options;
	po::store(po::parse_command_line(argc, argv, mainOptions), options);
	po::notify(options);

	if (options.count("help"))
	{
		cout << mainOptions << endl;
		exit(0);
	}

	return options;
}

int main(int argc, char* argv[])
{
	auto options = parseOptions(argc, argv);

	Node config = YAML::LoadFile(options["config-file"].as<string>());
	std::cout << "config:" << config << std::endl;

	asio::io_context io;
	Net::AsioDevice tcpreader(io, "irc.snoonet.org", 6667);
	Bot::IrcBot bot(tcpreader, config);

	io.run();
}
