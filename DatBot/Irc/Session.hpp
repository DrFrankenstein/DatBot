#pragma once

#include "../Net/AsioDevice.hpp"
#include "Models/Message.hpp"

#include <exception>
#include <rxcpp/rx.hpp>
#include <string>

namespace Irc
{
class Session
{
	public:
	Session(Net::AsioDevice& device, const std::string& nickname, const std::string& realname);

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
	rxcpp::subscription _messagesSub;
	rxcpp::subscription _states;
	rxcpp::composite_subscription _subscriptions;

	rxcpp::observable<Models::Message> _messages;
};
}
