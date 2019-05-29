#include "AsioDevice.hpp"

#include <cstdint>
#include <string>
#include <iterator>
#include <algorithm>

#include <boost/asio.hpp>

namespace Net {

using std::size_t;
using std::uint16_t;
using std::string;
using std::to_string;
using std::begin;
using std::end;
using std::find;

using boost::asio::async_connect;
using boost::asio::async_read_until;
using boost::asio::async_write;
using boost::asio::dynamic_buffer;
using boost::asio::ip::tcp;
using boost::asio::post;

using boost::system::error_code;

AsioDevice::AsioDevice(context& context, const string& host, uint16_t port)
    : DeviceBase(host, port), _context(context), _socket(context)
{ }

void AsioDevice::doSendRaw(const string& message)
{
    auto sender = [this, message]() {
        bool writing = _writeQueue.empty();
        _writeQueue.push(message + '\n');
        if (!writing)
            writeNext();
    };

    post(_context, sender);
}

void AsioDevice::doConnect()
{
    tcp::resolver resolver { _context };
    auto portstr = to_string(port());
    auto endpoints = resolver.resolve(host(), portstr);

    async_connect(_socket, endpoints,
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
    async_read_until(_socket, dynamic_buffer(_readBuf), '\n',
        [this](auto ec, auto size) { onRead(ec, size); }
    );
}

void AsioDevice::onRead(const error_code& err, size_t size)
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
    async_write(_socket, dynamic_buffer(_writeQueue.front()),
        [this](auto err, auto size) { onWrite(err, size); }
    );
}

void AsioDevice::onWrite(const error_code& err, size_t size)
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

string AsioDevice::extractLine(string& data)
{
    auto start = begin(data);
    auto delim = find(start, end(data), '\n') + 1;

    string line { start, delim };
    data.erase(start, delim);

    return line;
}

}