#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <poll.h>

// write in a more c++ way
// add classes
// add error management
// implement password
// implement commands
// handle exits gracefully
// tidy code
// smaller functions

std::string    nick(char *buffer)
{
    std::cout << "Storing nickname" << std::endl;
    std::string nickname;
    std::string message(buffer);
    //std::cout << "message = " << message << std::endl;
    size_t startPos = message.find("NICK ");
    if (startPos != std::string::npos)
    {
        size_t endPos = message.find("\n", startPos);
        if (endPos != std::string::npos)
        {
            nickname = message.substr(startPos + 5, endPos - (startPos + 6));
            //std::cout << "nickname = " << nickname << std::endl;
        }
    }
    return (nickname);
}

void    authenticate(char *buffer, std::string password, int clientSocket)
{
    std::cout << "Checking password" << std::endl;
    std::string message(buffer);
    //std::cout << "message = " << message << std::endl;
    size_t startPos = message.find("PASS ");
    if (startPos != std::string::npos)
    {
        size_t endPos = message.find("\n", startPos);
        if (endPos != std::string::npos)
        {
            std::string clientPassword = message.substr(startPos + 5, endPos - (startPos + 6));
            //std::cout << "clientPassword = " << clientPassword << std::endl;
            if (clientPassword != password)
            {
                send(clientSocket, "Error: Password incorrect\r\n", 26, 0);
                exit(1);
            }
            //else
            //    send(clientSocket, "Password correct\n", 17, 0);
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
        listen(serverSocket, 5); // Is this required?

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

                std::cout << "Client connected" << std::endl;

                char buffer[1024] = {0};
                ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                std::cout << buffer;
                //send(clientSocket, "HELLO\n", 6, 0);
                // NEED TO AUTHENTICATE PASSWORD HERE, AND pass, nick, user ?
                authenticate(buffer, password, clientSocket);
                nickname = nick(buffer);
                if (!nickname.empty())
                {
                    std::string response = "You are now known as " + nickname + "\r\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                }
                std::string welcomeMessage = ":server_name 001 " + nickname + " :Welcome to the IRC network\r\n"; //NEED TO FIX THIS
                send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);
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
                    //std::cout << "Message from client: " << buffer;
                    std::cout << buffer;
                    authenticate(buffer, password, pollfds[i].fd);
                    std::string temp = nickname;
                    nickname = nick(buffer);
                    if ((temp != nickname) && !nickname.empty())
                    {
                        std::string response = "You are now known as " + nickname + "\r\n";
                        send(pollfds[i].fd, response.c_str(), response.size(), 0);
                    }

                    //send(pollfds[i].fd, buffer, sizeof(buffer), 0);
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


