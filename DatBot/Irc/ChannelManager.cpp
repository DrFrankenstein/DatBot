#include "ChannelManager.hpp"

using Irc::Session;
using Irc::Models::Channel;
using std::string;

namespace Irc
{

ChannelManager::ChannelManager(Session& session):
    _session(session)
{
}

Channel ChannelManager::join(string name)
{
	return {};
}

}
