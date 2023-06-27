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
#define BroadcastCallback_t std::function<void(std::string &, ISocket *)>

namespace tcp
{
    class TCPServer
    {
    public:
        TCPServer();
        ~TCPServer();

    public:
        bool Listen(std::uint16_t port);
        void OnReceive(EndpointCallback_t callback);
        void OnBroadcast(BroadcastCallback_t callback);
        void Stop();

    private:
        std::unique_ptr<ISocket> _socket_ptr;

    private:
        std::mutex _response_mutex;
        EndpointCallback_t _handler;
        BroadcastCallback_t _broadcast_handler;
    };
}

#endif
