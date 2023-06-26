#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <functional>
#include <memory>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "tcp-server.hpp"
#include <sockets/epoll-socket.hpp>

using namespace tcp;
using namespace std::chrono_literals;

std::string connectAndSend(const std::string &address, int port, int &client_socket_id, const std::string &message)
{
    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return "";
    }

    // Set up the server address and port
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    if (inet_pton(AF_INET, address.c_str(), &(serverAddress.sin_addr)) <= 0)
    {
        std::cerr << "Failed to set server address." << std::endl;
        return "";
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        std::cerr << "Failed to connect to server." << std::endl;
        return "";
    }

    ssize_t bytesSent = send(clientSocket, message.c_str(), message.size(), 0);
    if (bytesSent < 0)
    {
        std::cerr << "Error while sending message to server." << std::endl;
        return "";
    }

    // Receive and print the response from the server
    char buffer[1024] = {0};
    std::string response;
    ssize_t bytesRead;
    size_t end_pos;

    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
    {
        response += buffer;
        memset(buffer, 0, sizeof(buffer));

        if ((end_pos = response.find(END_OF_TCP_STREAM)) != response.npos)
        {
            response.erase(end_pos, strlen(END_OF_TCP_STREAM));
            break;
        }
    }

    if (bytesRead < 0)
    {
        std::cerr << "Error while reading response from server." << std::endl;
        return "";
    }

    client_socket_id = clientSocket;

    return response;
}

TEST(TCPServerTest, SimpleServer)
{
    TCPServer server;
    std::thread{[&server]()
                {
                    server.On([=](std::string &request)
                              { return "OK:" + request; });
                    server.Listen(1234);
                }}
        .detach();

    // Wait for server warm up
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    const int NUMBER_OF_CONNECTIONS = 5000;
    std::vector<int> sockets(NUMBER_OF_CONNECTIONS);

    for (int i = 0; i < NUMBER_OF_CONNECTIONS; i++)
    {
        int client_socket = -1;
        auto response = connectAndSend("127.0.0.1", 1234, client_socket, "test-" + std::to_string(i));
        ASSERT_TRUE(client_socket >= 0);
        const std::string expected = "OK:test-" + std::to_string(i);
        ASSERT_TRUE(response == expected);
        sockets.push_back(client_socket);
    }

    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        close(sockets[i]);
    }
}
