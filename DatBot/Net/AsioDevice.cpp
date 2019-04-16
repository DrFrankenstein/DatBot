#include "AsioDevice.hpp"

#include <string>
#include <queue>
#include <iterator>
#include <boost/asio.hpp>

namespace Net {

namespace asio = boost::asio;
using boost::system::error_code;

AsioDevice::AsioDevice(context& context, const std::string& host, const::uint16_t port)
    : DeviceBase(host, port), _context(context), _socket(context)
{ }

void AsioDevice::doSendRaw(const std::string& message)
{
    auto sender = [this, message]() {
        bool writing = _writeQueue.empty();
        _writeQueue.push(message + '\n');
        if (!writing)
            writeNext();
    };

    asio::post(_context, sender);
}

void AsioDevice::doConnect()
{
    tcp::resolver resolver { _context };
    auto portstr = std::to_string(port());
    auto endpoints = resolver.resolve(host(), portstr);
    asio::async_connect(_socket, endpoints,
        [this](auto err, auto endpoint) { onConnect(err, endpoint); }
    );
}

void AsioDevice::onConnect(const error_code& err, tcp::endpoint&)
{
    setState(ConnectionState::ONLINE);
    readNext();
}

void AsioDevice::readNext()
{
    asio::async_read_until(_socket, asio::dynamic_buffer(_readBuf), '\n',
        [this](auto ec, auto size) { onRead(ec, size); }
    );
}

void AsioDevice::onRead(const error_code& err, std::size_t size)
{
    if (!err)
    {
        onMessage(extractLine(_readBuf));
        readNext();
    }
    else
    {
        onError();
        _socket.close();
    }
}

void AsioDevice::writeNext()
{
    asio::async_write(_socket, asio::dynamic_buffer(_writeQueue.front()),
        [this](auto err, auto size) { onWrite(err, size); }
    );
}

void AsioDevice::onWrite(const error_code& err, std::size_t size)
{
    if (err)
    {
        onError();
        _socket.close();
    }

    _writeQueue.pop();

    if (!_writeQueue.empty())
        writeNext();
}

std::string AsioDevice::extractLine(std::string& data)
{
    auto begin = std::begin(data);
    auto delim = std::find(begin, std::end(data), '\n') + 1;
    std::string line { begin, delim };
    data.erase(begin, delim);
    return line;
}

}