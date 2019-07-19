#include "Session.hpp"

#include "../Net/AsioDevice.hpp"
#include "Parsers/MessageParser.hpp"

#include <exception>
#include <iostream>
#include <rxcpp/rx.hpp>
#include <string>

namespace Irc
{
using Models::Message;
using Net::AsioDevice;
using Parsers::tryParseMessage;
using rxcpp::operators::map;
using std::exception_ptr, std::string;

Session::Session(AsioDevice& device):
    _device(device)
{
	_messages = _device.messages()
	    | map(tryParseMessage);

	_rawMessages = _messages.subscribe(
	    [this](auto message) { onMessage(message); },
	    [this](exception_ptr& ex) {},
	    [this]() {});

	_states = _device.states().subscribe(
	    [this](auto message) {},
	    [this](exception_ptr& ex) {},
	    [this]() {});
}

rxcpp::observable<Message> Session::messages()
{
	return _messages;
}

void Session::onMessage(const Message& message)
{
}
}
