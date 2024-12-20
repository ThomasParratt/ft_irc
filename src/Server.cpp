/*brief comment about Server*/

#include "Server.hpp"

bool servRunning = false;

Server::Server(std::string password, int port) : _password(password), _port(port), _welcomeSent(false)
{}

Server::~Server()
{
	// close serverSocker
	// close all the clientSockets
}

int Server::serverInit() {
	// create server socket
	_serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (_serverSocket == -1)
	{
		std::cerr << "Error creating socket" << std::endl;
		return -1;
	}
	// Set the SO_REUSEADDR option
	int opt = 1;
	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	// Define the server address
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(_port);
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	// bind server socket
	if (bind(_serverSocket, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr)) == -1)
	{
		std::cerr << "Error binding socket" << std::endl;
		return -2;
	}
	// listen server socket
	if (listen(_serverSocket, 5) == -1)
	{
		std::cerr << "Error listening on socket" << std::endl;
		return -3;
	}
	if (setServHostName() == -1)
	{
		std::cerr << "Error setting server hostname" << std::endl;
		return -4;
	}
	_startTime = time(nullptr); // saves the Server starting time
	_startTimeStr = getCurrentTime();
	std::cout << "Server started at " << _startTimeStr << std::endl;
	servRunning = true;	// set servRunning to true
	// return 1 for success
	return 1;
}

int Server::setServHostName()
{
    char hostname[MAX_LEN_HOSTNAME];
    if (gethostname(hostname, MAX_LEN_HOSTNAME) == -1)
    {
        std::cerr << "Error getting hostname" << std::endl;
        return -1;
    }
    // Ensure the hostname is null-terminated, if not, it means the hostname was too long and was truncated
    if (hostname[MAX_LEN_HOSTNAME - 1] != '\0')
    {
        std::cerr << "Hostname is too long, using host address instead" << std::endl;
        // Use the server address to get the host address
        char host_ip[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &(_serverAddr.sin_addr), host_ip, INET_ADDRSTRLEN) == nullptr)
        {
            std::cerr << "Error converting host address to string: " << strerror(errno) << std::endl;
            return -1;
        }
        _servHostName = host_ip;
    }
    else
    {
        _servHostName = hostname;
    }
	std::cout << "Server hostname set to " << _servHostName << std::endl;
    return 1;
}

void Server::acceptClient()
{
	int clientSocket = accept(_serverSocket, nullptr, nullptr);
	if (clientSocket == -1)
	{
		std::cerr << "Error accepting connection" << strerror(errno) << std::endl;
		return;
	}
	pollfd clientPollfd;
	clientPollfd.fd = clientSocket;
	clientPollfd.events = POLLIN | POLLOUT | POLLERR;
	pollfds.push_back(clientPollfd);
	clients.emplace_back(clientSocket, _password);
	std::cout << "New client connected, socket " << clientSocket << std::endl;
}

void Server::serverLoop() 
{
	// set serverPollfd
	pollfd serverPollfd;
	serverPollfd.fd = _serverSocket;
	serverPollfd.events = POLLIN | POLLERR;
	// push serverPollfd to pollfds
	pollfds.push_back(serverPollfd);
	std::cout << "Server loop started" << std::endl;

	// Map to store partial messages for each client
	std::map<int, std::string> clientBuffers;

	while (servRunning) 
	{
		if (poll(pollfds.data(), pollfds.size(), -1) == -1) 
		{
			std::cerr << "Error polling sockets " << strerror(errno) << std::endl;
			break;
		}
		if (pollfds[0].revents & POLLIN)
		{
			acceptClient();
		}
		for (int i = 1; i < pollfds.size(); i++)
		{
			if (pollfds[i].revents & POLLIN)
			{
				char buffer[1024] = {0};
				int bytesRead = recv(pollfds[i].fd, buffer, sizeof(buffer), 0);
				if (bytesRead <= 0) 
				{
					if (bytesRead == 0) 
						std::cout << "Client disconnected, socket " << pollfds[i].fd << std::endl;
					else 
						std::cerr << "Error reading from socket " << pollfds[i].fd << strerror(errno) << std::endl;
					close(pollfds[i].fd);
					pollfds.erase(pollfds.begin() + i);
					clients.erase(clients.begin() + (i - 1));
					clientBuffers.erase(pollfds[i].fd);
					i--;
					continue;
				}
				// Append received data to the client's buffer
				clientBuffers[pollfds[i].fd] += std::string(buffer);

				// Check for complete messages (assuming '\n' as a delimiter)
				std::string &clientBuffer = clientBuffers[pollfds[i].fd];
				size_t pos;
				while ((pos = clientBuffer.find('\n')) != std::string::npos)
				{
					// Extract the complete message
					std::string message = clientBuffer.substr(0, pos);
					clientBuffer.erase(0, pos + 1); // Remove the processed message

					std::cout << "Message received from socket " << pollfds[i].fd << ": " << message << std::endl;

					for (auto &client : clients)
					{
						if (client.getSocket() == pollfds[i].fd) 
						{
							if (this->messageHandler(message.c_str(), pollfds[i].fd, client) == 1)
							{
								std::cout << "Client disconnected, socket " << pollfds[i].fd << std::endl;
								close(pollfds[i].fd);
								pollfds.erase(pollfds.begin() + i);
								clients.erase(clients.begin() + (i - 1));
								clientBuffers.erase(pollfds[i].fd);
								i--;
								break;
							}
						}
					}
				}
			}
		}
	}

	close(serverPollfd.fd);
	for (auto &pfd : pollfds) 
	{
		close(pfd.fd);
	}
	std::cout << "Server stopped" << std::endl;
}


std::string getCurrentTime()
{
	time_t now = time(0);
	char timeStr[100];
    struct tm *localTime = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTime);
    return std::string(timeStr);
}

int		Server::getClientSocket(std::string nickname)
{
	int socket;

	for (int i = 0; i < this->clients.size(); i++)
	{
		if (nickname == clients[i].getNickname())
		{
			socket = clients[i].getSocket();
			return (socket);
		}
	}
	return (-2);//Return -2 to differentiate from -1 (failed socket)
}

