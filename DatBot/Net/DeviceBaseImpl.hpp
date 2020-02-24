#pragma once

#include "DeviceBase.hpp"

#include <rx.hpp>
#include <string>

namespace Net
{
template<class Impl>
DeviceBase<Impl>::DeviceBase(const std::string& host, const std::uint16_t port):
    _host(host), _port(port)
{
}

template<class Impl>
rxcpp::observable<std::string> DeviceBase<Impl>::messages()
{
	return _messages.get_observable();
}

template<class Impl>
rxcpp::observable<ConnectionState> DeviceBase<Impl>::states()
{
	return _state.get_observable();
}

template<class Impl>
void DeviceBase<Impl>::connect()
{
	impl().doConnect();
}

template<class Impl>
void DeviceBase<Impl>::sendRaw(const std::string& message)
{
	impl().doSendRaw(message);
}

template<class Impl>
std::string& DeviceBase<Impl>::host()
{
	return _host;
}

template<class Impl>
std::uint16_t DeviceBase<Impl>::port()
{
	return _port;
}

template<class Impl>
void DeviceBase<Impl>::setState(ConnectionState state)
{
	_state.get_subscriber().on_next(state);
}

template<class Impl>
void DeviceBase<Impl>::onMessage(const std::string& message)
{
	_messages.get_subscriber().on_next(message);
}

template<class Impl>
void DeviceBase<Impl>::onError()
{
	_messages.get_subscriber().on_error(std::current_exception());
}

template<class Impl>
Impl& DeviceBase<Impl>::impl()
{
	return *static_cast<Impl*>(this);
}

}
