#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

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

        // Set the SO_REUSEADDR option
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

        // Accept a client connection
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        while (true)
        {
            // Receive data from the client
            char buffer[1024] = {0};
            ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead == -1)
            {
                std::cerr << "Error reading from socket" << std::endl;
                break ;
            }
            else if (bytesRead == 0)
            {
                // Client closed connection
                std::cout << "Client disconnected" << std::endl;
                break ;
            }
            std::cout << buffer;
            authenticate(buffer, password, clientSocket);
            nickname = nick(buffer);
            if (!nickname.empty())
            {
                std::string response = "You are now known as " + nickname + "\r\n";
                send(clientSocket, response.c_str(), response.size(), 0);
                if (!welcomeSent)
                {
                    std::string welcomeMessage = ":ircserver 001 " + nickname + " :Welcome to the IRC network " + nickname + "\r\n"; //NEED TO FIX THIS
                    send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);
                    welcomeSent = true;
                }
            }
        }
        
        // Close the server socket
        close(clientSocket);
        close(serverSocket);
        std::cout << "Exiting server..." << std::endl;
    }
    else
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
}

