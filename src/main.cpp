#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Where should the while loop be?

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        u_int16_t port = std::stoi(argv[1]);
        std::string password = argv[2];

        // Create the server socket
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

        // Define the server address
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        // Bind the server socket
        bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

        // Listen for connections
        listen(serverSocket, 5);

        // Accept a client connection
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        // Receive data from the client
        char buffer[1024] = {0};
        recv(clientSocket, buffer, sizeof(buffer), 0);
        std::cout << "Message from client: " << buffer << std::endl;
        
        // Close the server socket
        close(serverSocket);
    }
    else
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
}