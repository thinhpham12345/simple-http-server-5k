#ifndef __TEST_HELPERS_HPP__
#define __TEST_HELPERS_HPP__

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "constants.hpp"

inline int connectToServer(const std::string &address, int port)
{
    // Create a socket
    int client_socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket_id < 0)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return -1;
    }

    // Set up the server address and port
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    if (inet_pton(AF_INET, address.c_str(), &(serverAddress.sin_addr)) <= 0)
    {
        std::cerr << "Failed to set server address." << std::endl;
        return -1;
    }

    // Connect to the server
    if (connect(client_socket_id, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        std::cerr << "Failed to connect to server." << std::endl;
        return -1;
    }

    return client_socket_id;
}

inline std::string getData(int client_socket_id, ssize_t &bytes)
{
    // Receive and print the response from the server
    char buffer[1024] = {0};
    std::string response;
    ssize_t bytesRead;
    size_t end_pos;

    while ((bytesRead = recv(client_socket_id, buffer, sizeof(buffer), 0)) > 0)
    {
        response += buffer;
        memset(buffer, 0, sizeof(buffer));

        bytes += bytesRead;

        if ((end_pos = response.find(END_OF_TCP_STREAM)) != response.npos)
        {
            response.erase(end_pos, strlen(END_OF_TCP_STREAM));
            break;
        }
    }

    return response;
}

inline int sendAndForget(int client_socket_id, const std::string &message)
{
    return send(client_socket_id, message.c_str(), message.size(), 0);
}

inline std::string sendMessage(int client_socket_id, const std::string &message)
{
    ssize_t bytesSent = sendAndForget(client_socket_id, message);
    if (bytesSent < 0)
    {
        std::cerr << "Error while sending message to server." << std::endl;
        return "";
    }

    ssize_t bytesRead = -1;
    auto response = getData(client_socket_id, bytesRead);

    if (bytesRead < 0)
    {
        std::cerr << "Error while reading response from server." << std::endl;
        return "";
    }

    return response;
}

inline std::string connectAndSend(const std::string &address, int port, int &client_socket_id, const std::string &message)
{
    auto _client_socket_id = connectToServer(address, port);

    if (_client_socket_id < 0)
    {
        return "";
    }

    auto response = sendMessage(_client_socket_id, message);

    if (response.empty())
    {
        return "";
    }

    client_socket_id = _client_socket_id;

    return response;
}

#endif
