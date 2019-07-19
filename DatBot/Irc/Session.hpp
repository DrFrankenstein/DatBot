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
	Session(Net::AsioDevice& device);

	rxcpp::observable<Models::Message> messages();

	private:
	void onMessage(const Models::Message& message);

	Net::AsioDevice& _device;
	rxcpp::subscription _rawMessages;
	rxcpp::subscription _states;

	rxcpp::observable<Models::Message> _messages;
};
}
