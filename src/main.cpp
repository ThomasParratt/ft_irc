#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

// write in a more c++ way
// add classes
// add poll
// add error management

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        u_int16_t port = std::stoi(argv[1]);
        std::string password = argv[2];

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
            send(clientSocket, "Enter message: \n", 15, 0);
            recv(clientSocket, buffer, sizeof(buffer), 0);
            std::cout << "Message received: " << buffer;
            if (strncmp(buffer, "EXIT", 4) == 0)
                break ;
        }
        
        // Close the server socket
        close(clientSocket);
        close(serverSocket);
        std::cout << "Exiting server..." << std::endl;
    }
    else
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
}

