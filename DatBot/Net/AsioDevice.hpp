#pragma once

#include "DeviceBase.hpp"

#include <string>
#include <queue>
#include <boost/asio.hpp>

namespace Net {

class AsioDevice : public DeviceBase<AsioDevice>
{
    using context = boost::asio::io_context;
    using tcp = boost::asio::ip::tcp;

public:
    AsioDevice(context& context, const std::string& host, const::uint16_t port);
    
    void doSendRaw(const std::string& message);
    void doConnect();

private:
    void onConnect(const boost::system::error_code& err, tcp::endpoint&);

    void readNext();
    void onRead(const boost::system::error_code& err, std::size_t size);

    void writeNext();
    void onWrite(const boost::system::error_code& err, std::size_t size);

    std::string extractLine(std::string& data);

    context& _context;
    tcp::socket _socket;

    std::queue<std::string> _writeQueue;
    std::string _readBuf;
};

}