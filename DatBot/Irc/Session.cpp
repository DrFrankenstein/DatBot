#include "Session.hpp"

#include "../Net/AsioDevice.hpp"
#include "Parsers/MessageParser.hpp"

#include <exception>
#include <iostream>
#include <rx.hpp>
#include <string>
#include <utility>

namespace Irc
{
using Models::Message;
using Net::AsioDevice, Net::ConnectionState;
using Parsers::tryParseMessage;
using rxcpp::operators::map, rxcpp::operators::filter;
using std::cout, std::endl, std::exception_ptr, std::move, std::string;

Session::Session(AsioDevice& device, string nickname, string realname):
    _nickname(move(nickname)), _realname(move(realname)), _device(device), _channels(*this)
{
	// for debugging purposes
	_debugSubscription = _device.messages().subscribe(
	    [](auto message) { cout << "<<< " << message; });

	_messages = _device.messages()
	    | map(tryParseMessage);

	_messagesSubscription = _messages.subscribe(
	    [this](auto message) { onMessage(message); },
	    [](exception_ptr& /*ex*/) {},
	    []() {});

	_statesSubscription = _device.states().subscribe(
	    [this](auto state) { onState(state); },
	    [](exception_ptr& /*ex*/) {},
	    []() {});

	_pingsSubscription = messagesOfType("PING").subscribe(
	        [this](auto message) { onPing(message); },
	        [](exception_ptr& /*ex*/) {},
	        []() {});
}

void Session::start()
{
	_device.connect();
}

rxcpp::observable<Message> Session::messages()
{
	return _messages;
}

rxcpp::observable<Message> Session::messagesOfType(const Message::Command& command)
{
	// TODO: this function exists so that we can have a central lookup map to dispatch
	// messages instead of a bunch of duplicate filters to run through. but consider this
	// a stub for now.
	return _messages | filter([&](const Message& message) { return message.command == command; });
}

void Session::send(const Message& message)
{
	string raw = message.toString();
	cout << ">>> " << raw << endl;
	_device.sendRaw(raw);
}

// Retrieves the session's nickname.
const string& Session::nickname() const
{
	return _nickname;
}

// Changes the nickname for this session.
// @param newnick The new nickname. Must be a valid nickname.
// @remarks See [rfc2812 2.3.1] for requirements of a valid nickname.
void Session::nickname(const string& newnick)
{
	_nickname = newnick;
	send({ "NICK", { _nickname } });
}

// Retrieves the user's realname
const string& Session::realname() const
{
	return _realname;
}

// Changes the user's realname.
// @remarks This has no effect server-side past the registration stage.
void Session::realname(const string& newname)
{
	_realname = newname;
}

void Session::onMessage(const Message& message)
{
}

void Session::onState(ConnectionState state)
{
	switch (state)
	{
	case ConnectionState::ONLINE:
		send({ "NICK", { _nickname } });
		send({ "USER", { _nickname, "2", "*", _realname } });
		break;
	case ConnectionState::OFFLINE:;  // nothing for now
	}
}

void Session::onPing(const Message& message)
{
	string value = message.params[0];
	Message reply { "PONG", { value } };
	send(reply);
}

}
