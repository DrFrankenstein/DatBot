#pragma once

#include "../Net/AsioDevice.hpp"
#include "ChannelManager.hpp"
#include "Models/Message.hpp"

#include <exception>
#include <rx.hpp>
#include <string>

namespace Irc
{
class Session
{
	public:
	Session(Net::AsioDevice& device, const std::string& nickname, const std::string& realname);

	void start();

	rxcpp::observable<Models::Message> messages();
	void send(const Models::Message& message);

	const std::string& nickname() const;
	void nickname(const std::string& newnick);

	const std::string& realname() const;
	void realname(const std::string& newname);

	private:
	void onMessage(const Models::Message& message);
	void onState(Net::ConnectionState state);

	void onPing(const Models::Message& message);

	std::string _nickname;
	std::string _realname;

	Net::AsioDevice& _device;

	rxcpp::composite_subscription _messagesSubscription;
	rxcpp::composite_subscription _statesSubscription;
	rxcpp::composite_subscription _debugSubscription;
	rxcpp::composite_subscription _pingsSubscription;

	rxcpp::observable<Models::Message> _messages;

	ChannelManager _channels;
};
}
