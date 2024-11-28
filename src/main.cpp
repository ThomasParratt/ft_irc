#include <iostream>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

std::vector<int> clients;
std::mutex client_mutex;


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

int    handleMessages(char *buffer, std::string password, std::string nickname, int clientSocket, bool welcomeSent)
{
    std::string clientPassword = messageParam(buffer, "PASS ");
    if (!clientPassword.empty())
    {
        if (clientPassword != password)
        {
            send(clientSocket, "Error: Wrong password\r\n", 23, 0);
            return (1);
        }
    }
    std::string capMessage = messageParam(buffer, "CAP ");
    if (!capMessage.empty())
    {
        std::string response = "CAP * LS :\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
    }
    nickname = messageParam(buffer, "NICK ");
    if (!nickname.empty())
    {
        std::string response = "You are now known as " + nickname + "\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
        if (!welcomeSent)
        {
            std::string welcomeMessage = ":ircserver 001 " + nickname + " :Welcome to the IRC network " + nickname + "\r\n";
            send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);
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

void clientHandle(int clientSocket)
{
    char buffer[1024];
    while (true) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(clientSocket, buffer, sizeof(buffer));
        if (bytes_read <= 0) {
            std::cout << "Client disconnected\n";
            close(clientSocket);
            // Remove client from the list
            std::lock_guard<std::mutex> lock(client_mutex);
            clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
            return;
        }

        std::cout << "Message received: " << buffer;

        // Broadcast message to all other clients
        std::lock_guard<std::mutex> lock(client_mutex);
        for (int client : clients) {
            if (client != clientSocket) {
                send(client, buffer, bytes_read, 0);
            }
        }
    }
}
int main(int argc, char **argv)
{
    if (argc == 3)
    {
        u_int16_t port = std::stoi(argv[1]);
        std::string password = argv[2];
        std::string nickname;
        bool welcomeSent = false;
        // Create the server socket
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            std::cerr << "Error creating socket" << std::endl;
            return -1;
        }
        // Set the SO_REUSEADDR option
        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        // Define the server address
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        // Bind the server socket (associate the socket with a port on local machine so listen knows which port to listen to)
        if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            std::cerr << "Error binding socket" << std::endl;
            return -2;
        }

        // Listen (wait) for connections with 5 incoming connections allowed in queue
        if (listen(serverSocket, 5) == -1) {
            std::cerr << "Error listening on socket" << std::endl;
            return -3;
        }
        //set up the pollfd struct for the server socket
        std::vector<pollfd> pollfds;
        pollfd serverPollfd;
        serverPollfd.fd = serverSocket;
        serverPollfd.events = POLLIN | POLLERR;
        pollfds.push_back(serverPollfd);
        std::map<int, sockaddr_in> clients;
        while (true) 
        {
            std::cout << "Server started" << std::endl;
            if (poll(pollfds.data(), pollfds.size(), -1) == -1) 
            {
                std::cerr << "Error polling sockets" << strerror(errno) << std::endl;
                return -4;
            }
            std::vector<pollfd>::iterator it = pollfds.begin();
            while (it != pollfds.end())
            {
                if (it->revents & POLLIN)
                {
                    if (it->fd == serverSocket)
                    { 
                        // Accepting new client
                        sockaddr_in clientAddress;
                        socklen_t clientAddressSize = sizeof(clientAddress);
                        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
                        if (clientSocket == -1) {
                            std::cerr << "Error accepting connection" << strerror(errno) << std::endl;
                            return -5;
                        } else {
                            pollfd clientPollfd;
                            clientPollfd.fd = clientSocket;
                            clientPollfd.events = POLLIN | POLLOUT | POLLERR;
                            pollfds.push_back(clientPollfd);
                            clients[clientSocket] = clientAddress;
                            std::cout << "Client connected" << std::endl;
                        }
                    }
                    else
                    {
                        /* 29_11 couldn't figure out why the file descriptor is always bad when trying to use poll()*/
                        // handling data from client
                        std::cout << "Handling data from client" << std::endl;
                        if (fcntl(it->fd, F_GETFD) == -1 && errno == EBADF)
                        {
                            std::cerr << "Invalid file descriptor: " << it->fd << std::endl;
                            close(it->fd);
                            clients.erase(it->fd);
                            it = pollfds.erase(it);
                            return -6;
                        }
                        char buffer[1024] = {0};
                        int bytesRead = recv(it->fd, buffer, sizeof(buffer), 0);
                        if (bytesRead <= 0) 
                        {
                            if (bytesRead == 0) 
                                std::cout << "Client disconnected\n";
                            else 
                                std::cerr << "Error reading from socket" << strerror(errno) << std::endl;
                            close(it->fd);
                            clients.erase(it->fd);
                            it = pollfds.erase(it);
                            continue;
                        } 
                        else 
                        {
                            std::cout << "Message received: " << buffer;
                            int ret = handleMessages(buffer, password, nickname, it->fd, welcomeSent);
                            if (ret == 1)
                            {
                                close(it->fd);
                                clients.erase(it->fd);
                                it = pollfds.erase(it);
                                continue;
                            }
                        }
                    }
                }
                else if (it->events & POLLERR) 
                {
                    std::cerr << "Error polling socket" << strerror(errno) <<std::endl;
                    close(it->fd);
                    clients.erase(it->fd);
                    it = pollfds.erase(it);
                    continue;
                }
                ++it;
            }
        }
        std::cout << "Exiting server..." << std::endl; 
        for (const auto& client : clients) 
        {
            close(client.first);
        }
        close (serverSocket);
        // // Accept a client connection
        // int clientSocket = accept(serverSocket, nullptr, nullptr);

        // while (true)
        // {
        //     // Receive data from the client
        //     char buffer[1024] = {0};
        //     ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        //     if (bytesRead == -1)
        //     {
        //         std::cerr << "Error reading from socket" << std::endl;
        //         break ;
        //     }
        //     else if (bytesRead == 0)
        //     {
        //         // Client closed connection
        //         std::cout << "Client disconnected" << std::endl;
        //         break ;
        //     }
        //     std::cout << buffer;
        //     int ret = handleMessages(buffer, password, nickname, clientSocket, welcomeSent);
        //     if (ret == 1) 
        //         break ;
        //     else if (ret == 2)
        //         welcomeSent = true;
        // }
        // // Close the server socket
        // close(clientSocket);
        // close(serverSocket);
        // std::cout << "Exiting server..." << std::endl;
    }
    else
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
    return (0);
}

