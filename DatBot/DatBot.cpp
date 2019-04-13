#include <iostream>
#include <string>
#include <functional>
#include <algorithm>

#include <boost/asio.hpp>
#include <rxcpp/rx.hpp>

#include "AsioDevice.hpp"

namespace asio = boost::asio;

class IrcBot
{
public:
    IrcBot(AsioDevice& reader)
        : _reader(reader)
    { 
       _messagesSub = _reader.messages().subscribe(
            [this](auto message) { onMessage(message); },
            [this](std::exception_ptr& ex) { onError(ex); },
            [this]() { onComplete(); }
       );

       _statesSub = _reader.states().subscribe(
            [this](auto state) { onState(state); },
            [this](std::exception_ptr& ex) { onError(ex); },
            [this]() { onComplete(); }
       );

       reader.connect();
    }

private:
    void onState(ConnectionState state)
    {
        switch (state)
        {
        case ConnectionState::ONLINE:
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

    AsioDevice& _reader;
    rxcpp::subscription _messagesSub;
    rxcpp::subscription _statesSub;
};

int main()
{
    asio::io_context io;
    AsioDevice tcpreader (io, "irc.snoonet.org", 6667);
    IrcBot bot (tcpreader);

    io.run();
}
