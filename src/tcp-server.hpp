#ifndef __TCP_SERVER_HPP__
#define __TCP_SERVER_HPP__

#include <string>
#include <functional>
#include <map>
#include <memory>
#include <mutex>

#include "./constants.hpp"
#include "./sockets/isocket.hpp"

#define EndpointCallback_t std::function<std::string(std::string &)>

namespace tcp
{
    class TCPServer
    {
    public:
        TCPServer();
        ~TCPServer();

    public:
        bool Listen(std::uint16_t port);
        void On(EndpointCallback_t endpoint_callback);
        void Stop();

    private:
        std::unique_ptr<ISocket> _socket_ptr;

    private:
        std::mutex _response_mutex;
        EndpointCallback_t _handler;
    };
}

#endif
