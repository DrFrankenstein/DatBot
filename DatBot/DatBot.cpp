#include "Net/AsioDevice.hpp"

#include <string>
#include <boost/asio.hpp>

#include "Irc/Session.hpp"

namespace asio = boost::asio;
using Irc::Session;
using std::string;

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

	private:
	Session _session;
};
}

int main()
{
	asio::io_context io;
	Net::AsioDevice tcpreader(io, "irc.snoonet.org", 6667);
	Bot::IrcBot bot(tcpreader, "DrFrankTest", "DatBot Test User");

	io.run();
}
