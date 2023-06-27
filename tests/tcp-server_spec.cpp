#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <vector>

#include "tcp-server.hpp"
#include <sockets/epoll-socket.hpp>
#include "./_helpers/helpers.hpp"

using namespace tcp;
using namespace std::chrono_literals;

TEST(TCPServerTest, SimpleServer)
{
    std::thread{[]()
                {
                    TCPServer server;
                    server.OnReceive([=](std::string &request)
                                     { return "OK:" + request; });
                    server.Listen(1234);
                }}
        .detach();

    // Wait for server warm up
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    const int NUMBER_OF_CONNECTIONS = 5000;
    std::vector<int> sockets;
    sockets.reserve(NUMBER_OF_CONNECTIONS);

    for (int i = 0; i < NUMBER_OF_CONNECTIONS; i++)
    {
        int client_socket = -1;
        auto response = connectAndSend("127.0.0.1", 1234, client_socket, "test-" + std::to_string(i));
        ASSERT_TRUE(client_socket >= 0);
        const std::string expected = "OK:test-" + std::to_string(i);
        ASSERT_TRUE(response == expected);
        sockets.emplace_back(client_socket);
    }

    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        close(sockets[i]);
    }
}

TEST(TCPServerTest, SimpleBroadcastingServer)
{
    std::thread{[]()
                {
                    TCPServer server(true);
                    server.OnReceive([=](std::string &request)
                                     { return request; });
                    server.Listen(1234);
                }}
        .detach();

    // Wait for server warm up
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    const int NUMBER_OF_CONNECTIONS = 10; // minimal connections to avoid DDOS attack
    std::vector<int> sockets;
    sockets.reserve(NUMBER_OF_CONNECTIONS);

    int first_client_socket = connectToServer("127.0.0.1", 1234);
    sockets.emplace_back(first_client_socket);

    for (int i = 1; i < NUMBER_OF_CONNECTIONS; i++)
    {
        int client_socket = connectToServer("127.0.0.1", 1234);
        ASSERT_TRUE(client_socket >= 0);
        sockets.emplace_back(client_socket);
    }

    sendAndForget(sockets[0], "Hello World!");
    ssize_t bytes;

    for (int i = 1; i < NUMBER_OF_CONNECTIONS; i++)
    {
        auto data = getData(sockets[i], bytes);
        ASSERT_TRUE(data == "Hello World!");
    }

    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        close(sockets[i]);
    }
}
