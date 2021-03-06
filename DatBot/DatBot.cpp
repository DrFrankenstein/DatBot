#include "Bot/IrcBot.hpp"
#include "Net/AsioDevice.hpp"

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>

namespace asio = boost::asio;
namespace po   = boost::program_options;
using std::cerr, std::cout, std::endl, std::exception, std::exit, std::string, std::uint16_t;
using YAML::Node;

po::variables_map parseOptions(int argc, char** argv)
{
	po::options_description mainOptions { "Main options" };

	// clang-format off
	mainOptions.add_options()
		("help,?",                                                           "print this help")
		("config-file,f", po::value<string>()->default_value("datbot.conf"), "set config file path")
	;
	// clang-format on

	po::variables_map options;
	po::store(po::parse_command_line(argc, argv, mainOptions), options);
	po::notify(options);

	if (options.count("help") > 0)
	{
		cout << mainOptions << endl;
		exit(0);
	}

	return options;
}

int main(int argc, char* argv[])
{
	try
	{
		constexpr uint16_t DEFAULT_PORT = 6667;

		auto options = parseOptions(argc, argv);
		Node config = YAML::LoadFile(options["config-file"].as<string>());
		std::cout << "config:" << config << std::endl;

		asio::io_context io;
		Net::AsioDevice tcpreader(io, "irc.snoonet.org", DEFAULT_PORT);
		Bot::IrcBot bot(tcpreader, config);

		bot.start();
		io.run();
	}
	catch (exception& ex)
	{
		cerr << "ERROR: " << ex.what() << endl;
		return EXIT_FAILURE;
	}
	catch (...)
	{
		cerr << "Unknown ERROR" << endl;
		return EXIT_FAILURE;
	}
}
