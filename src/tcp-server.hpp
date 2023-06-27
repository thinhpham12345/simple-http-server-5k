#ifndef __TCP_SERVER_HPP__
#define __TCP_SERVER_HPP__

#include <string>
#include <functional>
#include <map>
#include <memory>
#include <mutex>

#include "./constants.hpp"
#include "./sockets/isocket.hpp"

#define DataCallback_t std::function<std::string(std::string &)>

namespace tcp
{
    class TCPServer
    {
    public:
        TCPServer(bool enable_broadcast = false);
        ~TCPServer();

    public:
        bool Listen(std::uint16_t port);
        void OnReceive(DataCallback_t callback);
        void Stop();

    private:
        std::unique_ptr<ISocket> _socket_ptr;
        bool _is_broadcasting;

    private:
        std::mutex _response_mutex;
        DataCallback_t _handler;
    };
}

#endif
