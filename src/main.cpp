#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <poll.h>
#include <chrono>
#include <ctime>
#include "client.hpp"

// write in a more c++ way
// add classes
// add error management
// implement password
// implement commands
// handle exits gracefully
// tidy code
// smaller functions

std::string messageParam(char *buffer, std::string message)
{
    std::string param;
    std::string received(buffer);
    size_t startPos = received.find(message);
    if (startPos != std::string::npos)
    {
        size_t endPos = received.find("\n", startPos);
        if (endPos != std::string::npos)
            param = received.substr(startPos + 5, endPos - (startPos + 6));
    }
    return (param);
}

int    handleMessages(char *buffer, int clientSocket, Client client)
{
    std::cout << "Handling messages" << std::endl;
    std::string clientPassword = messageParam(buffer, "PASS ");
    if (!clientPassword.empty())
    {
        if (clientPassword != client.getPassword())
        {
            send(clientSocket, "Error: Wrong password\r\n", 23, 0);
            return (1);
        }
    }
    client.setNickname(messageParam(buffer, "NICK "));
    if (!client.getNickname().empty())
    {
        std::string response = "You are now known as " + client.getNickname() + "\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
        if (!client.getWelcomeSent())
        {
            std::string message_001 = ":ircserv 001 " + client.getNickname() + " :Welcome to the IRC network " + client.getNickname() + "\r\n";
            send(clientSocket, message_001.c_str(), message_001.size(), 0);
            std::string message_002 = ":ircserv 002 " + client.getNickname() + " :Your host ircserv, running version 1.0\r\n";
            send(clientSocket, message_002.c_str(), message_002.size(), 0);
            return (2);
        }
    }
    std::string ping = messageParam(buffer, "PING ");
    if (!ping.empty())
    {
        std::string response = "PONG " + ping + "\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
    }
    return (0);
}

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        u_int16_t port = std::stoi(argv[1]);
        Client client(argv[2], false);

        // Create the server socket
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

        // Set the SO_REUSEADDR option to allow resuse of the port
        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // Define the server address
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        // Bind the server socket (associate the socket with a port on local machine so listen knows which port to listen to)
        bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

        // Listen (wait) for connections with 5 incoming connections allowed in queue
        listen(serverSocket, 5);

        // Set up poll structures
        std::vector<struct pollfd> pollfds;
        std::vector<int> clientSockets;
        pollfds.push_back({serverSocket, POLLIN, 0}); // Add server socket to poll with the event POLLIN (data available to read)

        while (true)
        {
            // Continuously monitors sockets for events using poll()
            int pollCount = poll(pollfds.data(), pollfds.size(), -1);
            // Check the server socket for new incoming connections
            if (pollfds[0].revents & POLLIN)
            {
                // Accept a client 
                int clientSocket = accept(serverSocket, nullptr, nullptr);

                // Add the new client socket to the poll set
                pollfds.push_back({clientSocket, POLLIN, 0});
                clientSockets.push_back(clientSocket);

                std::cout << "Client " << clientSockets.size() << " connected" << std::endl;

                char buffer[1024] = {0};
                ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                std::cout << "********* 1 **********" << std::endl;
                std::cout << buffer;
                int ret = handleMessages(buffer, clientSocket, client);
                if (ret == 1) 
                    break ;
                else if (ret == 2)
                    client.setWelcomeSent(true);
            }

            // Handle client communication by iterating through the client sockets and check for incoming data
            for (size_t i = 1; i < pollfds.size(); i++)
            {
                if (pollfds[i].revents & POLLIN)
                {
                    char buffer[1024] = {0};
                    ssize_t bytesRead = recv(pollfds[i].fd, buffer, sizeof(buffer), 0);

                    if (bytesRead == -1)
                    {
                        std::cerr << "Error reading from socket" << std::endl;
                        close(pollfds[i].fd);
                        pollfds.erase(pollfds.begin() + i);
                        clientSockets.erase(clientSockets.begin() + i);
                        i--;
                        continue ;
                    }
                    else if (bytesRead == 0)
                    {
                        // Client closed connection
                        std::cout << "Client disconnected" << std::endl;
                        close(pollfds[i].fd);
                        pollfds.erase(pollfds.begin() + i);
                        clientSockets.erase(clientSockets.begin() + i);
                        i--;
                        continue ;
                    }
                    // Process the message from the client
                    std::cout << "********* 2 **********" << std::endl;
                    std::cout << buffer;
                    int ret = handleMessages(buffer, clientSockets[i], client);
                    if (ret == 1) 
                        break ;
                    else if (ret == 2)
                        client.setWelcomeSent(true);
                }
            }
        }
        close(serverSocket);
        for (size_t i = 1; i < pollfds.size(); i++)
        {
            close(pollfds[i].fd);
            close(clientSockets[i]);
        }
        std::cout << "Exiting server..." << std::endl;
    }
    else
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
}


