#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <iostream>
#include <vector>
#include <functional>
#include <list>

#define ConnectionHandler_t std::function<void(int socket_id, std::string &req)>

namespace tcp
{
    class ISocket
    {
    public:
        virtual int CreateSocket(std::uint16_t port) = 0;
        virtual void OnReceived(ConnectionHandler_t handler) = 0;

    public:
        virtual bool Recv(int client_socket_id, char *buffer, size_t buffer_size, ssize_t &read_bytes) = 0;
        virtual void Send(int client_socket_id, const std::string &content) = 0;
        virtual void Close(int client_socket_id) = 0;
        virtual void Close() = 0;

    public:
        virtual bool IsListening() = 0;
        virtual std::list<int> Clients() = 0;
    };
}

#endif
