#pragma once

#include "DeviceBase.hpp"

#include <boost/asio.hpp>
#include <queue>
#include <string>

namespace Net
{
class AsioDevice : public DeviceBase<AsioDevice>
{
	using context = boost::asio::io_context;
	using tcp     = boost::asio::ip::tcp;

	using error_code = boost::system::error_code;

	public:
	AsioDevice(context& context, const std::string& host, const ::uint16_t port);

	void doSendRaw(const std::string& message);
	void doConnect();

	private:
	void onConnect(const error_code& err, tcp::endpoint&);

	void readNext();
	void onRead(const error_code& err, std::size_t size);

	void writeNext();
	void onWrite(const error_code& err, std::size_t size);

	std::string extractLine(std::string& data);

	context& _context;
	tcp::socket _socket;

	std::queue<std::string> _writeQueue;
	std::string _readBuf;
};

}