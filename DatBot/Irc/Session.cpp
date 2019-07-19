#include "Session.hpp"

#include <exception>
#include <string>
#include <iostream>
#include <rxcpp/rx.hpp>

#include "../Net/AsioDevice.hpp"
#include "Parsers/MessageParser.hpp"

namespace Irc
{
    using std::exception_ptr, std::string;
    using rxcpp::operators::map;
    using Net::AsioDevice;
    using Models::Message;
    using Parsers::tryParseMessage;

    Session::Session(AsioDevice& device)
        : _device(device)
    {
        _messages = _device.messages()
            | map(tryParseMessage);

        _rawMessages = _messages.subscribe(
            [this](auto message) { onMessage(message); },
            [this](exception_ptr& ex) {},
            [this]() {}
        );

        _states = _device.states().subscribe(
            [this](auto message) {},
            [this](exception_ptr& ex) {},
            [this]() {}
        );
    }

    rxcpp::observable<Message> Session::messages()
    {
        return _messages;
    }

    void Session::onMessage(const Message& message)
    {

    }
}
