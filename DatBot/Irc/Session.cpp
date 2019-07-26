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
using Net::AsioDevice, Net::ConnectionState;
using Parsers::tryParseMessage;
using rxcpp::operators::map, rxcpp::operators::filter, rxcpp::operators::subscribe;
using std::cout, std::endl, std::exception_ptr, std::string;

Session::Session(AsioDevice& device, const string& nickname, const string& realname):
    _device(device), _nickname(nickname), _realname(realname)
{
	_messages = _device.messages()
	    | map(tryParseMessage);

	_messagesSub = _messages.subscribe(
	    [this](auto message) { onMessage(message); },
	    [this](exception_ptr& ex) {},
	    [this]() {});

	_states = _device.states().subscribe(
	    [this](auto state) { onState(state); },
	    [this](exception_ptr& ex) {},
	    [this]() {});

	auto pings = _messages
	    | filter([](const Message& message) { return message.is("PING"); });
	(void) _subscriptions.add(pings.subscribe(
	    [this](auto message) { onPing(message); },
	    [this](exception_ptr& ex) {},
	    [this]() {}));
}

rxcpp::observable<Message> Session::messages()
{
	return _messages;
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
	// for debugging purposes
	cout << "<<< " << message << endl;
}

void Session::onState(ConnectionState state)
{
	switch (state)
	{
	case ConnectionState::ONLINE:
		send({ "NICK", { _nickname } });
		send({ "USER", { _nickname, "2", "*", _realname } });
		break;
	}
}

void Session::onPing(const Message& message)
{
	string value = message.params[0];
	Message reply { "PONG", { value } };
	send(reply);
}

}
