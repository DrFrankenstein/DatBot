#pragma once

#include "Models/Channel.hpp"
#include "Models/Events.hpp"
#include "Models/Message.hpp"

#include <rx.hpp>
#include <string>
#include <unordered_map>

namespace Irc
{
class Session;

class ChannelManager
{
	public:
	ChannelManager(Session& session);

	rxcpp::observable<Models::Channel> join(std::string name);

	rxcpp::observable<Models::Channel> joins() const;
	rxcpp::observable<Models::PartEvent> parts() const;

	private:
	void onJoin(const Models::Message& message);
	void onPart(const Models::Message& message);

	Session& _session;
	std::unordered_map<std::string, Models::Channel> _channels;

	rxcpp::subjects::subject<Models::Channel> _joins;
	rxcpp::subjects::subject<Models::PartEvent> _parts;

	rxcpp::composite_subscription _joinsSub, _partsSub;
};

}
