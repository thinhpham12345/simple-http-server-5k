#include "./tcp-server.hpp"
#include "./log.hpp"
#include "./utils/thread-pool.hpp"
#include "./sockets/epoll-socket.hpp"

using ThreadsPool = utils::ThreadsPool;

namespace tcp
{
    TCPServer::TCPServer(bool enable_broadcast)
    {
        _socket_ptr = std::make_unique<EpollSocket>(MAX_ASYNC_EVENTS_QUEUE_SIZE);
        _is_broadcasting = enable_broadcast;
    }

    TCPServer::~TCPServer()
    {
        log(INFO) << "Closing socket \n";
    }

    bool TCPServer::Listen(std::uint16_t port)
    {
        // auto number_of_threads_support = std::thread::hardware_concurrency();
        auto number_of_threads_support = THREADS_POOL_SIZE;
        ThreadsPool pool(number_of_threads_support);

        if (_handler)
        {
            _socket_ptr->OnReceived([&pool, this](int socket_id, std::string &req)
                                    {
                                        log(INFO) << "OnConnection socket_id " << socket_id << " data " << req << "\n";

                                        pool.Enqueue([this, socket_id, &req]() {
                                            auto response = _handler(req);

                                            if (_is_broadcasting)
                                            {
                                                for (auto &client_id : _socket_ptr->Clients())
                                                {
                                                    if (client_id != socket_id)
                                                    {
                                                        _socket_ptr->Send(client_id, response + END_OF_TCP_STREAM);
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                _socket_ptr->Send(socket_id, response + END_OF_TCP_STREAM);
                                            }
                                        }); });
        }

        if (_socket_ptr->CreateSocket(port) == CLOSED_SOCKET_ID)
        {
            return false;
        }
        log(INFO) << "Listening at port " << port << "\n";

        while (_socket_ptr->IsListening())
        {
            // log(INFO) << "Running...\n";
        }

        return true;
    }

    void TCPServer::OnReceive(DataCallback_t callback)
    {
        _handler = callback;
    }

    void TCPServer::Stop()
    {
        _socket_ptr->Close();
    }
}
