#include <iostream>
#include <string>
#include <functional>
#include <algorithm>

#include <boost/asio.hpp>
#include <rxcpp/rx.hpp>

#include "Net/AsioDevice.hpp"

namespace asio = boost::asio;

namespace Bot {
class IrcBot
{
public:
    IrcBot(Net::AsioDevice& reader)
        : _reader(reader)
    {
        _messagesSub = _reader.messages().subscribe(
            [this](auto message) { onMessage(message); },
            [this](std::exception_ptr & ex) { onError(ex); },
            [this]() { onComplete(); }
        );

        _statesSub = _reader.states().subscribe(
            [this](auto state) { onState(state); },
            [this](std::exception_ptr & ex) { onError(ex); },
            [this]() { onComplete(); }
        );

        reader.connect();
    }

private:
    void onState(Net::ConnectionState state)
    {
        switch (state)
        {
        case Net::ConnectionState::ONLINE:
            _reader.sendRaw("USER DrFrankTest 0 * :Dr Frankenstein");
            _reader.sendRaw("NICK DrFrankTest");
            break;
        }
    }

    void onMessage(const std::string& message)
    {
        std::cout << "<<< " << message;
    }

    void onError(std::exception_ptr& ex)
    { }

    void onComplete()
    {
        std::cout << "DONE" << std::endl;
    }

    Net::AsioDevice& _reader;
    rxcpp::subscription _messagesSub;
    rxcpp::subscription _statesSub;
};
}

int main()
{
    asio::io_context io;
    Net::AsioDevice tcpreader(io, "irc.snoonet.org", 6667);
    Bot::IrcBot bot(tcpreader);

    io.run();
}