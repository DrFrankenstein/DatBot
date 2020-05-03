#include "ChannelManager.hpp"

#include "Models/Channel.hpp"
#include "Models/Events.hpp"
#include "Models/Message.hpp"
#include "Session.hpp"

#include <rx.hpp>
#include <string>

using Irc::Session;
using Irc::Models::Channel, Irc::Models::Message, Irc::Models::PartEvent;
using rxcpp::observable, rxcpp::operators::filter, rxcpp::operators::first;
using std::move, std::string;

namespace Irc
{
ChannelManager::ChannelManager(Session& session):
    _session(session)
{
	_joinsSub = session.messagesOfType("JOIN").subscribe([this](auto& message) { onJoin(message); });
	_partsSub = session.messagesOfType("PART").subscribe([this](auto& message) { onPart(message); });
}

observable<Channel> ChannelManager::join(string name)
{
	// FIXME: rxcpp doesn't like observables of references, I think.
	// I don't think we want to pass around copies of channels, so... what to do? pointers?
	return joins()
	    | filter([=](auto& channel) { return channel.name == name; })
	    | first();
}

observable<Channel> ChannelManager::joins() const
{
	return _joins.get_observable();
}

observable<PartEvent> ChannelManager::parts() const
{
	return _parts.get_observable();
}

void ChannelManager::onJoin(const Message& message)
{
	if (message.params.empty())
	{
		// TODO handle error: malformed join message
	}

	auto name = message.params[0];
	Channel channel { name };
	auto [item, inserted] = _channels.insert({ name, move(channel) });

	if (!inserted)
	{
		// TODO handle error: joining channel we thought we'd already joined
	}

	_joins.get_subscriber().on_next(item->second);
}

void ChannelManager::onPart(const Message& message)
{
	if (message.params.empty())
	{
		// TODO handle error: malformed part message
	}

	auto& name = message.params[0];
	string reason;

	if (message.params.size() > 1)
		reason = message.params[1];

	_channels.erase(name);

	_parts.get_subscriber().on_next(PartEvent { name, reason });
}

}
