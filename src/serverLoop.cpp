#include "Server.hpp"

void Server::acceptClient()
{
	int clientSocket = accept(_serverSocket, nullptr, nullptr);
	if (clientSocket == -1)
	{
		std::cerr << "Error accepting connection " << strerror(errno) << std::endl;
		return;
	}
	int flags = fcntl(clientSocket, F_GETFL, 0);
    if (flags == -1 || fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK) == -1) 
	{
        std::cerr << "Error setting client socket to non-blocking mode: " << strerror(errno) << std::endl;
        close(clientSocket);
        return ;
    }
	pollfd clientPollfd = {};
	clientPollfd.fd = clientSocket;
	clientPollfd.events = POLLIN;
	_pollfds.push_back(clientPollfd);
	_clients.emplace_back(clientSocket, _password);
	std::cout << "New client connected, socket " << clientSocket << std::endl;
}

/*
	Checks if Client requests to connect to server, and then accepts client.
*/
void	Server::checkServerSocket()
{
	if (_pollfds[0].revents & POLLIN)	//Check for clients trying to connect
	{
		acceptClient();
	}
}


void	Server::setUpServerPollfd()
{
	pollfd			serverPollfd;

	serverPollfd.fd = _serverSocket;
	serverPollfd.events = POLLIN;

	_pollfds.push_back(serverPollfd);
}


/*
	Main serverloop that checks (and responds to) events on Server Socket and Client socket(s).
	Loop ends with "CTRL + C" signal.
*/
void Server::serverLoop() 
{
	std::cout << "Server loop started" << std::endl;

	std::map<int, std::string> clientBuffers;	// Map to store partial messages for each client


	setUpServerPollfd();

	while (servRunning) 
	{
		if (poll(_pollfds.data(), _pollfds.size(), -1) == -1)		//Check file descriptors
		{
			if (errno != EINTR) //no signal received
				std::cerr << "Error polling sockets " << strerror(errno) << std::endl;
			break ;
		}
		checkServerSocket();

		checkClientSockets(clientBuffers);
	}

}
