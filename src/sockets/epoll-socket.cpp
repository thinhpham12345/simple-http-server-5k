#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "./epoll-socket.hpp"
#include "./log.hpp"

namespace tcp
{
    EpollSocket::EpollSocket(int max_events)
    {
        _max_events = max_events;
    }

    EpollSocket::~EpollSocket()
    {
        for (auto &client_id : _connected_clients)
        {
            Close(client_id);
        }
    }

    int EpollSocket::_CreateSocket(std::uint16_t port)
    {
        int socket_id = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, SOCKET_PROTOCOL);

        if (socket_id < 0)
        {
            log(ERROR) << "Failed to create a socket"
                       << "\n";
            return CLOSED_SOCKET_ID;
        }

        // Set socket options
        int opt = 1;
        if (setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
        {
            log(ERROR) << "Failed to set socket options\n";
            return CLOSED_SOCKET_ID;
        }

        sockaddr_in server_address{};
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_port = htons(port);

        if (bind(socket_id, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        {
            log(ERROR) << "Unable to bind with port " << port << "\n";
            return CLOSED_SOCKET_ID;
        }

        if (listen(socket_id, MAX_CONNECTIONS) < 0)
        {
            log(ERROR) << "Unable to start server with number of connections: " << MAX_CONNECTIONS << "\n";
            return CLOSED_SOCKET_ID;
        }

        return socket_id;
    }

    int EpollSocket::CreateSocket(std::uint16_t port)
    {
        _socket_id = _CreateSocket(port);

        if (_socket_id < 0)
        {
            return CLOSED_SOCKET_ID;
        }

        _epoll_fd = epoll_create1(0);

        if (_epoll_fd == -1)
        {
            log(ERROR) << "Cannot create epoll \n";
            Close(_socket_id);
            return CLOSED_SOCKET_ID;
        }

        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = _socket_id;

        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _socket_id, &event) == -1)
        {
            log(ERROR) << "Cannot start edge-triggered mode\n";
            Close(_socket_id);
            return CLOSED_SOCKET_ID;
        }

        _is_listening = true;
        log(INFO) << "Server Socket ID " << _socket_id << "\n";

        return _socket_id;
    }

    bool EpollSocket::IsListening()
    {
        epoll_event events[_max_events];
        int events_count = epoll_wait(_epoll_fd, events, _max_events, -1);

        if (events_count == -1)
        {
            _is_listening = false;
            return _is_listening;
        }

        for (int event_idx = 0; event_idx < events_count; ++event_idx)
        {
            auto event = events[event_idx];
            int socket_id = event.data.fd;

            if (socket_id == _socket_id)
            {
                _HandleNewConnection(socket_id);
            }
            else
            {
                if (event.events & EPOLLHUP)
                {
                    Close(socket_id);
                }
                else
                {
                    _HandleIncomingData(socket_id);
                }
            }
        }

        return _is_listening;
    }

    void EpollSocket::OnReceived(ConnectionHandler_t handler)
    {
        _connection_handler = handler;
    }

    bool EpollSocket::Recv(int client_socket_id, char *buffer, size_t buffer_size, ssize_t &read_bytes)
    {
        read_bytes = recv(client_socket_id, buffer, buffer_size, 0);
        return read_bytes > 0;
    }

    void EpollSocket::Send(int client_socket_id, const std::string &content)
    {
        log(INFO) << "Sending back data to client id " << client_socket_id << " content " << content << "\n";
        if (send(client_socket_id, content.c_str(), content.size(), MSG_NOSIGNAL) == -1)
        {
            log(ERROR) << "Unable to send " << content << " because socket is unreachable or closed id " << client_socket_id << "\n";
        }
    }

    void EpollSocket::Close(int client_socket_id)
    {
        log(INFO) << "Closing client_socket_id: " << client_socket_id << "\n";
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_socket_id, nullptr) == -1)
        {
            log(ERROR) << "Cannot close socket id " << client_socket_id << "\n";
            log(ERROR) << "EpollSocket Cannot close socket id " << client_socket_id << "\n";
        }
        _connected_clients.remove(client_socket_id);
    }

    void EpollSocket::Close()
    {
        Close(_epoll_fd);
    }

    void EpollSocket::_HandleNewConnection(int server_socket_id)
    {
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        int client_socket_id = accept4(server_socket_id, (struct sockaddr *)&addr, &addr_len, SOCK_NONBLOCK);
        log(INFO) << "New connection client_socket_id " << client_socket_id << " errno " << errno << "\n";
        if (client_socket_id == -1)
        {
            log(ERROR) << "Invalid client socket id " << client_socket_id << "\n";
            return;
        }

        if (fcntl(client_socket_id, F_SETFL, O_NONBLOCK) == -1)
        {
            log(ERROR) << "Cannot set client socket id " << client_socket_id << " to non-blocking mode\n";
            return;
        }

        struct epoll_event event;
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = client_socket_id;
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_socket_id, &event) == -1)
        {
            log(ERROR) << "Cannot enable edge-triggered mode for client socket id " << client_socket_id << "\n";
            return;
        }

        _connected_clients.push_back(client_socket_id);
    }

    void EpollSocket::_HandleIncomingData(int client_socket_id)
    {
        char buffer[BUFFER_SIZE];
        std::string data;
        ssize_t bytes = -1;

        while (Recv(client_socket_id, buffer, BUFFER_SIZE - 1, bytes) && bytes)
        {
            // terminate string buffer for string append
            buffer[bytes] = 0;

            data += buffer;

            if (data.find(END_OF_TCP_STREAM) != data.npos)
            {
                break;
            }
        }

        if (data.size() >= 0)
        {
            log(INFO) << "data: " << data << "\n";
            if (_connection_handler)
                _connection_handler(client_socket_id, data);
            return;
        }

        if (data.empty() || errno != EWOULDBLOCK)
        {
            Close(client_socket_id);
        }
    }
}
