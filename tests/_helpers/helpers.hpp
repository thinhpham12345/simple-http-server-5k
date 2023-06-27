#ifndef __TEST_HELPERS_HPP__
#define __TEST_HELPERS_HPP__

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "constants.hpp"

inline std::string connectAndSend(const std::string &address, int port, int &client_socket_id, const std::string &message)
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

#endif
