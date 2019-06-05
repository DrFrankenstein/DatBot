#pragma once

#include <exception>
#include <string>
#include <rxcpp/rx.hpp>

#include "../Net/AsioDevice.hpp"
#include "Models/Message.hpp"

namespace Irc
{
    class Session
    {
    public:
        Session(Net::AsioDevice& device);

        rxcpp::observable<Models::Message> messages();

    private:
        void onMessage(const std::string& message);

        Net::AsioDevice& _device;
        rxcpp::subscription _rawMessages;
        rxcpp::subscription _states;

        rxcpp::observable<Models::Message> _messages;
    };
}
