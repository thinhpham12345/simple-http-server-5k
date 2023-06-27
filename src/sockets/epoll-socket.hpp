#ifndef __EPOLL_SOCKET_HPP__
#define __EPOLL_SOCKET_HPP__

#include <sys/epoll.h>

#include "./constants.hpp"
#include "./isocket.hpp"

namespace tcp
{
    class EpollSocket : public ISocket
    {
    public:
        EpollSocket() = delete;
        explicit EpollSocket(int max_events);
        ~EpollSocket();

    public:
        int CreateSocket(std::uint16_t port) override;
        void OnReceived(ConnectionHandler_t handler) override;
        void OnBroadcast(ConnectionHandler_t handler) override;

    public:
        bool Recv(int client_socket_id, char *buffer, size_t buffer_size, ssize_t &read_bytes) override;
        void Send(int client_socket_id, const std::string &content) override;
        void Close(int client_socket_id) override;
        void Close() override;

    public:
        bool IsListening() override;

    private:
        int _CreateSocket(std::uint16_t port);
        void _HandleNewConnection(int server_socket_id);
        void _HandleIncomingData(int client_socket_id);

    private:
        int _socket_id = CLOSED_SOCKET_ID;
        bool _is_listening;
        ConnectionHandler_t _connection_handler;
        ConnectionHandler_t _broadcast_handler;

    private:
        int _epoll_fd;
        int _max_events;
        std::list<int> _connected_clients;
    };
}

#endif
