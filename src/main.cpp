// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <signal.h>
// #include <fcntl.h>

// volatile sig_atomic_t signalReceived = 0;

// static void handleSignal(int signal)
// {
//     std::cout << "Signal received" << std::endl;
//     signalReceived = 1;
// }

/*int main(int argc, char **argv)
{
    if (argc == 3)
    {
        u_int16_t port = std::stoi(argv[1]);
        std::string password = argv[2];

        // Create the server socket
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

        // Set socket to non-blocking
        //fcntl(serverSocket, F_SETFL, O_NONBLOCK);

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

        // Set socket to non-blocking
        // fcntl(clientSocket, F_SETFL, O_NONBLOCK);

        //signal(SIGINT, handleSignal);
        while (true)
        {
            // Receive data from the client
            char buffer[1024] = {0};
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            std::cout << "Message from client: " << buffer << std::endl;
            //send(clientSocket, buffer, bytesReceived, 0);
        }
        
        // Close the server socket
        close(serverSocket);
        std::cout << "Shutting down..." << std::endl;
    }
    else
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
}*/

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

        while (true)
        {
            // Receive data from the client
            char buffer[1024] = {0};
            recv(clientSocket, buffer, sizeof(buffer), 0);
            std::cout << "Message from client: " << buffer << std::endl;
        }
        
        // Close the server socket
        close(serverSocket);
    }
    else
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
}

// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <signal.h>
// #include <fcntl.h>
// #include <poll.h>

// volatile sig_atomic_t signalReceived = 0;

// static void handleSignal(int signal)
// {
//     std::cout << "Signal received" << std::endl;
//     signalReceived = 1;
// }

// int main(int argc, char **argv)
// {
//     if (argc == 3)
//     {
//         u_int16_t port = std::stoi(argv[1]);
//         std::string password = argv[2];

//         // Create the server socket
//         int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

//         if (serverSocket < 0) {
//             std::cerr << "Socket creation failed" << std::endl;
//             return 1;
//         }

//         // Set socket to non-blocking
//         fcntl(serverSocket, F_SETFL, O_NONBLOCK);

//         // Define the server address
//         sockaddr_in serverAddress;
//         serverAddress.sin_family = AF_INET;
//         serverAddress.sin_port = htons(port);
//         serverAddress.sin_addr.s_addr = INADDR_ANY;

//         // Bind the server socket
//         if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
//             std::cerr << "Bind failed" << std::endl;
//             return 1;
//         }

//         // Listen for connections
//         if (listen(serverSocket, 5) < 0) {
//             std::cerr << "Listen failed" << std::endl;
//             return 1;
//         }

//         // Setup signal handler
//         signal(SIGINT, handleSignal);

//         // Setup pollfd for server socket (to detect incoming connections)
//         struct pollfd fds[1];
//         fds[0].fd = serverSocket;
//         fds[0].events = POLLIN;  // We are interested in incoming connections

//         while (!signalReceived)
//         {
//             // Use poll to wait for events on the server socket
//             int pollResult = poll(fds, 1, 1000); // Timeout of 1000 ms (1 second)

//             if (pollResult < 0) {
//                 std::cerr << "Poll failed" << std::endl;
//                 break;
//             }

//             // Check if the server socket is ready for accepting a connection
//             if (fds[0].revents & POLLIN) {
//                 // Accept a client connection
//                 int clientSocket = accept(serverSocket, nullptr, nullptr);

//                 if (clientSocket < 0) {
//                     std::cerr << "Accept failed" << std::endl;
//                     continue;
//                 }

//                 std::cout << "Client connected" << std::endl;

//                 // Set up poll for the client socket
//                 struct pollfd clientFd;
//                 clientFd.fd = clientSocket;
//                 clientFd.events = POLLIN;  // We are interested in data to read

//                 // Monitor the client socket for incoming data
//                 while (!signalReceived)
//                 {
//                     pollResult = poll(&clientFd, 1, 1000);  // Timeout of 1000 ms

//                     if (pollResult < 0) {
//                         std::cerr << "Poll failed" << std::endl;
//                         break;
//                     }

//                     if (clientFd.revents & POLLIN) {
//                         // Data is available to read from the client
//                         char buffer[1024] = {0};
//                         int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                        
//                         if (bytesReceived == 0) {
//                             std::cout << "Client disconnected" << std::endl;
//                             break;
//                         } else if (bytesReceived < 0) {
//                             std::cerr << "Error receiving data" << std::endl;
//                             break;
//                         }

//                         std::cout << "Message from client: " << buffer << std::endl;
//                         // Optionally send data back to client here
//                         // send(clientSocket, buffer, bytesReceived, 0);
//                     }
//                 }

//                 close(clientSocket);  // Close client socket after handling communication
//             }
//         }

//         // Close the server socket after finishing
//         close(serverSocket);
//         std::cout << "Shutting down..." << std::endl;
//     }
//     else {
//         std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
//     }

//     return 0;
// }
