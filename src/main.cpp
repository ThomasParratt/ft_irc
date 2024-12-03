#include "Msg.hpp"
#include "Server.hpp"




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

int main(int argc, char **argv)
{
    Server	server;
	
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
			std::cout << "******* 1 ********" << std::endl;
			std::cout << buffer;
			std::cout << "******* 2 ********" << std::endl;
			server.messageHandler(buffer);
            int ret = handleMessages(buffer, password, nickname, clientSocket, welcomeSent);
            if (ret == 1)
                break ;
            else if (ret == 2)
                welcomeSent = true;
        }
        // Close the server socket
        close(clientSocket);
        close(serverSocket);
        std::cout << "Exiting server..." << std::endl;
    }
    else
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
}

