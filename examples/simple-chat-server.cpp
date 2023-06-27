#include <iostream>
#include <memory>

#include <tcp-server.hpp>
#include <sockets/isocket.hpp>
#include <log.hpp>

using namespace tcp;

int main(int, char **)
{
    log(INFO) << "Starting simple TCP server \n";

    TCPServer server(true);

    server.OnReceive([=](std::string &request)
                     { return request; });

    server.Listen(8080);

    return 0;
}
