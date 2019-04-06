#include <iostream>
#include <string>
#include <functional>
#include <algorithm>
#include <exception>
#include <queue>

#include <boost/asio.hpp>

#include <rxcpp/rx.hpp>

namespace asio = boost::asio;
using asio::ip::tcp;
namespace rx = rxcpp;

enum class ConnectionState
{
    ONLINE,
    OFFLINE
};

class TcpReader {
public:
    TcpReader(asio::io_context& context, const std::string& host, const std::string& port)
        : _context(context), _socket(context), _host(host), _port(port)
    {
        _packets = rx::observable<>::create<std::string>(
            [this](auto subscriber) { onSubscribe(subscriber); }
        );
    }

    rx::observable<std::string> messages()
    {
        _packets.subscribe(_messages.get_subscriber());
        return _messages.get_observable();
    }

    rx::observable<ConnectionState> states()
    {
        return _state.get_observable();
    }

    void sendRaw(const std::string& message)
    {
        asio::post(_context, [this, message]() {
            bool writing = _writeQueue.empty();
            _writeQueue.push(message + '\n');
            if (!writing)
                writeNext();
        });
    }
private:
    void onSubscribe(rx::subscriber<std::string> subscriber)
    {
        connect();
    }

    void connect()
    {
        tcp::resolver resolver (_context);
        auto endpoints = resolver.resolve(_host, _port);
        asio::async_connect(_socket, endpoints, 
            [this](auto err, auto endpoint) { onConnect(err, endpoint); }
        );
    }

    void onConnect(boost::system::error_code& err, tcp::endpoint&)
    {
        if (!err)
            std::cout << "Connected" << std::endl;
        else
            std::cout << "Connect error " << err << std::endl;

        _state.get_subscriber().on_next(ConnectionState::ONLINE);

        readNext();
    }

    void readNext()
    {
        asio::async_read_until(_socket, asio::dynamic_buffer(_readBuf), '\n',
            [this](auto ec, auto size) { onMessage(ec, size); }
        );
    }

    std::string extractLine(std::string & data)
    {
        auto begin = std::begin(data);
        auto delim = std::find(begin, std::end(data), '\n') + 1;
        std::string line { begin, delim };
        data.erase(begin, delim);
        return line;
    }

    void onMessage(const boost::system::error_code& err, std::size_t size)
    {
        if (!err)
        {
            _messages.get_subscriber().on_next(extractLine(_readBuf));
            readNext();
        }
        else
        {
            std::cout << "Socket error " << err.message() << std::endl;
            _messages.get_subscriber().on_error(std::current_exception());
            _socket.close();
        }
    }

    void writeNext()
    {
        asio::async_write(_socket, asio::dynamic_buffer(_writeQueue.front()),
            [this](auto err, auto size) { onWrite(err, size); }
        );
    }

    void onWrite(const boost::system::error_code& err, std::size_t size)
    {

        if (!err)
        {
            std::cout << ">>> " << _writeQueue.front() << std::endl;
        }
        else
        {
            std::cout << "Socket error " << err.message() << std::endl;
            _messages.get_subscriber().on_error(std::current_exception());
            _socket.close();
        }

        _writeQueue.pop();

        if (!_writeQueue.empty())
            writeNext();
    }

    std::string _host;
    std::string _port;

    asio::io_context& _context;
    tcp::socket _socket;

    rx::observable<std::string> _packets;
    rx::subjects::subject<std::string> _messages;
    rx::subjects::subject<ConnectionState> _state;

    std::queue<std::string> _writeQueue;
    std::string _readBuf;
};

template <typename Reader>
class IrcBot
{
public:
    IrcBot(Reader& reader)
        : _reader(reader)
    { 
        _reader.messages().subscribe(
            [this](auto message) { onMessage(message); },
            [this](std::exception_ptr& ex) { onError(ex); },
            [this]() { onComplete(); }
        );

        _reader.states().subscribe(
            [this](auto state) { onState(state); },
            [this](std::exception_ptr& ex) { onError(ex); },
            [this]() { onComplete(); }
        );
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

    Reader& _reader;
};

int main()
{
    asio::io_context io;
    TcpReader tcpreader (io, "irc.snoonet.org", "6667");
    IrcBot<TcpReader> bot (tcpreader);

    io.run();
}
