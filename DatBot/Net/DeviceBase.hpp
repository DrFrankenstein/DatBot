#pragma once

#include <cstdint>
#include <rx.hpp>
#include <string>

namespace Net
{
enum class ConnectionState
{
	ONLINE,
	OFFLINE
};

template<class Impl>
class DeviceBase
{
	public:
	DeviceBase(const std::string& host, const std::uint16_t port);

	rxcpp::observable<std::string> messages();
	rxcpp::observable<ConnectionState> states();

	void connect();

	void sendRaw(const std::string& message);

	protected:
	std::string& host();
	std::uint16_t port();
	void setState(ConnectionState state);
	void onMessage(const std::string& message);
	void onError();

	private:
	Impl& impl();

	std::string _host;
	std::uint16_t _port;

	rxcpp::subjects::subject<std::string> _messages;
	rxcpp::subjects::subject<ConnectionState> _state;
};

}

#include "DeviceBaseImpl.hpp"
