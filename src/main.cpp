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

std::vector<int> clients;
std::mutex client_mutex;


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
// int main(int argc, char **argv)
// {
//     if (argc == 3)
//     {
//         u_int16_t port = std::stoi(argv[1]);
//         std::string password = argv[2];
//         std::string nickname;
//         bool welcomeSent = false;
//         // Create the server socket
//         int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//         if (serverSocket == -1) {
//             std::cerr << "Error creating socket" << std::endl;
//             return -1;
//         }
//         // Set the SO_REUSEADDR option
//         int opt = 1;
//         setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
//         // Define the server address
//         sockaddr_in serverAddress;
//         serverAddress.sin_family = AF_INET;
//         serverAddress.sin_port = htons(port);
//         serverAddress.sin_addr.s_addr = INADDR_ANY;
//         // Bind the server socket (associate the socket with a port on local machine so listen knows which port to listen to)
//         if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
//             std::cerr << "Error binding socket" << std::endl;
//             return -2;
//         }
//         // Listen (wait) for connections with 5 incoming connections allowed in queue
//         if (listen(serverSocket, 5) == -1) {
//             std::cerr << "Error listening on socket" << std::endl;
//             return -3;
//         }
//         //set up the pollfd struct for the server socket
//         std::vector<int> clientSockets;
//         std::vector<pollfd> pollfds;
//         pollfd serverPollfd;
//         serverPollfd.fd = serverSocket;
//         serverPollfd.events = POLLIN | POLLERR;
//         pollfds.push_back(serverPollfd);
//         while (true) 
//         {
//             // std::cout << "Server started" << std::endl;
//             // std::cout << pollfds.size() << std::endl;
//             if (poll(pollfds.data(), pollfds.size(), -1) == -1) 
//             {
//                 std::cerr << "Error polling sockets " << strerror(errno) << std::endl;
//                 return -4;
//             }

//             if (pollfds[0].revents & POLLIN)
//             {
//                 // Accepting new client
//                 int clientSocket = accept(serverSocket, nullptr, nullptr);
//                 if (clientSocket == -1) {
//                     std::cerr << "Error accepting connection" << strerror(errno) << std::endl;
//                     return -5;
//                 }
//                 pollfd clientPollfd;
//                 clientPollfd.fd = clientSocket;
//                 clientPollfd.events = POLLIN | POLLOUT | POLLERR;
//                 pollfds.push_back(clientPollfd);
//                 clientSockets.push_back(clientSocket);
//                 std::cout << "Client connected" << std::endl;
//                 char buffer[1024] = {0};
//                 int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
//                 if (bytesRead <= 0) {
//                     if (bytesRead == 0) 
//                         std::cout << "Client disconnected\n";
//                     else 
//                         std::cerr << "Error reading from socket " << strerror(errno) << std::endl;
//                     break;
//                 }
//                 std::cout << "Message received: " << buffer << std::endl;
//                 int ret = handleMessages(buffer, password, nickname, clientSocket, welcomeSent);
//                 if (ret == 1)
//                 {
//                     std::cout << "Client disconnected**\n";
//                     break ;
//                 } else if (ret == 2)
//                     welcomeSent = true;
//             }
//             // for (int i = 1; i < pollfds.size(); i++)
//             // {
//             //     if (pollfds[i].revents & POLLIN)
//             //     {
//             //         char buffer[1024] = {0};
//             //         int bytesRead = recv(pollfds[i].fd, buffer, sizeof(buffer), 0);
//             //         if (bytesRead <= 0) {
//             //             if (bytesRead == 0) 
//             //                 std::cout << "Client disconnected\n";
//             //              else 
//             //                 std::cerr << "Error reading from socket " << strerror(errno) << std::endl;
//             //             close(pollfds[i].fd);
//             //             pollfds.erase(pollfds.begin() + i);
//             //             clientSockets.erase(clientSockets.begin() + i);
//             //             i--;
//             //             continue;
//             //         }
//             //         std::cout << "Message received: " << buffer;
//             //         int ret = handleMessages(buffer, password, nickname, clientSockets[i], welcomeSent);
//             //         if (ret == 1)
//             //         {
//             //             std::cout << "Client disconnected**\n";
//             //             break ;
//             //         } else if (ret == 2)
//             //             welcomeSent = true;
//             //     }
//             // }
//         }
//         close(serverSocket);
//         for (size_t i = 1; i < pollfds.size(); i++)
//         {
//             close(pollfds[i].fd);
//             close(clientSockets[i]);
//         }
//         std::cout << "Exiting server..." << std::endl; 
//     }
//     else
//         std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
// }
