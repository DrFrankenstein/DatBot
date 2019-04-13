#pragma once

#include <cstdint>
#include <string>
#include <exception>
#include <rxcpp/rx.hpp>

enum class ConnectionState
{
    ONLINE, OFFLINE
};

template <class Impl>
class DeviceBase
{
public:
    DeviceBase(const std::string& host, const std::uint16_t port)
        : _host(host), _port(port)
    { }

    rxcpp::observable<std::string> messages()
    {
        return _messages.get_observable();
    }
    rxcpp::observable<ConnectionState> states() { return _state.get_observable(); }

    void connect() { impl().doConnect(); }

    void sendRaw(const std::string& message) { impl().doSendRaw(message); }

protected:
    std::string& host() { return _host; }
    std::uint16_t port() { return _port; }
    void setState(ConnectionState state) { _state.get_subscriber().on_next(state); }
    void onMessage(const std::string& message) { _messages.get_subscriber().on_next(message); }
    void onError() { _messages.get_subscriber().on_error(std::current_exception()); }

private:
    Impl& impl() { return *static_cast<Impl*>(this); }

    std::string _host;
    std::uint16_t _port;

    rxcpp::observable<std::string> _packets;
    rxcpp::subjects::subject<std::string> _messages;
    rxcpp::subjects::subject<ConnectionState> _state;
};