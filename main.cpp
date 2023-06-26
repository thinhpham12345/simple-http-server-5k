#include <iostream>
#include <memory>

#include "./tcp-server.hpp"
#include "./log.hpp"

using namespace tcp;

int main(int, char **)
{
    log(INFO) << "Starting simple TCP server \n";

    TCPServer server;

    server.On([=](std::string &request)
              { return "OK\n"; });

    server.Listen(8080);

    return 0;
}
