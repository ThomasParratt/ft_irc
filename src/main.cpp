#include <iostream>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "Server.hpp"

int main(int argc, char **argv)
{
    if (argc ==3) {
        u_int16_t port = std::stoi(argv[1]);
        Server server(argv[2], port);
        if (server.serverInit() != 1) {
            std::cout << "Server initialization failed" << std::endl;
            return -1;
        }
        server.serverLoop();
    } else {
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
    }
    return 0;
}
