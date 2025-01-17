/*brief comment about Server*/

#include "Server.hpp"

bool servRunning = false;

Server::Server(std::string password, int port) : _password(password), _welcomeSent(false), _port(port)
{}

Server::~Server()
{
	// close serverSocker
	// close all the clientSockets
}

int Server::serverInit() 
{
	// create server socket
	// _serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // SOCK_NONBLOCK is not supported on macOS
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
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
	_pollfds.push_back(clientPollfd);
	_clients.emplace_back(clientSocket, _password);
	std::cout << "New client connected, socket " << clientSocket << std::endl;
}

void	Server::disconnectClient(size_t& i, std::map<int, std::string> &clientBuffers)
{
	close(_pollfds[i].fd);
	_pollfds.erase(_pollfds.begin() + i);
	_clients.erase(_clients.begin() + (i - 1));
	clientBuffers.erase(_pollfds[i].fd);
	i--;
}

void Server::removeUserFromAllChannels(int i)
{
	std::cout << "removeFromAll" << std::endl;
	for (auto &client : _clients) // remove user from all channels
	{
		if (client.getSocket() == _pollfds[i].fd) 
		{
			std::string userPrefix = client.getPrefix();
			for (auto &channel : _channel_names)
			{
				for (auto &user : channel.getChannelUsers())
				{
					if (client.getNickname() == user.nickname)
					{
						int j = getChannelIndex(channel.getChannelName(), _channel_names);
						std::string message = "REMOVE " + user.nickname + " from " + channel.getChannelName();
						std::cout << "This user being removed: " << userPrefix << std::endl;
						std::string quitMessage = ":" + userPrefix + " QUIT " + ":Client has quit\r\n";
						// std::string quitMessage = ":" + user.nickname + " QUIT " + ":Client has quit\r\n";
						broadcastToChannel(this->_channel_names[j], quitMessage, client, 1);
						removeUser(user.nickname, channel.getChannelName(), message, 2);
						client.leaveChannel(channel.getChannelName());
					}
				}
			}
		}
	}
}

/*
	Checks if Client is requests to connect to server, and then accepts client.
*/
void	Server::checkServerSocket()
{
	if (_pollfds[0].revents & POLLIN)	//Check for clients trying to connect
	{
		acceptClient();
	}
}

void		Server::printErrorBytesRead(int	bytesRead, int i)
{
	if (bytesRead == 0)
	{
		std::cout << "Client disconnected, socket " << _pollfds[i].fd << std::endl;
	}
	else 
		std::cerr << "Error reading from socket " << _pollfds[i].fd << " " << strerror(errno) << std::endl;
}

void	Server::removeClient(int bytesRead, size_t i, std::map<int, std::string> &clientBuffers)
{
	printErrorBytesRead(bytesRead, i);
	removeUserFromAllChannels(i);
	disconnectClient(i, clientBuffers);
}

void	Server::processClientBuffer(size_t i, std::map<int, std::string> &clientBuffers)
{
	std::string &clientBuffer = clientBuffers[_pollfds[i].fd];	// Check for complete messages (assuming '\n' as a delimiter)

	size_t		pos;
	bool		clientDisconnected = false;

	while ((pos = clientBuffer.find('\n')) != std::string::npos)
	{
		std::string message = clientBuffer.substr(0, pos);				// Extract the complete message
		clientBuffer.erase(0, pos + 1);									// Remove the processed message

		std::cout << "Message received from socket " << _pollfds[i].fd << ": " << message << std::endl;

		for (auto &client : _clients)
		{
			if (client.getSocket() == _pollfds[i].fd) 
			{
				if (this->makeSelectAndRunCommand(message.c_str(), client) == 1)
				{
					std::cout << "Client disconnected, socket " << _pollfds[i].fd << std::endl;
					disconnectClient(i, clientBuffers);//get rid of Client struct??
					clientDisconnected = true;
					break ;
				}
			}
		}
		if (clientDisconnected)
			break ;
	}
}


void	Server::checkClientSockets(std::map<int, std::string> &clientBuffers)
{
	for (size_t i = 1; i < _pollfds.size(); i++)		//Loop through all Client sockets
	{
		if (_pollfds[i].revents & POLLIN)		//If Event occurred in socket
		{
			char buffer[1024] = {0};
			int bytesRead = recv(_pollfds[i].fd, buffer, sizeof(buffer), 0);
			if (bytesRead <= 0) 
			{
				removeClient(bytesRead, i, clientBuffers);
				continue ;
			}
			clientBuffers[_pollfds[i].fd] += std::string(buffer); 	//Append received data to the client's buffer

			processClientBuffer(i, clientBuffers);
		}
	}
}

void	Server::setUpServerPollfd()
{
	pollfd			serverPollfd;

	serverPollfd.fd = _serverSocket;
	serverPollfd.events = POLLIN | POLLERR;		//Server configured to incoming events and error.

	_pollfds.push_back(serverPollfd);
}
/*
	Closes file descriptors (Fds) found in pollfds.
*/
void	Server::closeFDs()
{
	std::cout << "Closing File Descriptors" << std::endl;
	for (pollfd &pollfd : _pollfds) 
	{
		close(pollfd.fd);
	}
}

void Server::serverLoop() 
{
	std::cout << "Server loop started" << std::endl;

	std::map<int, std::string> clientBuffers;	// Map to store partial messages for each client


	setUpServerPollfd();

	while (servRunning) 
	{
		if (poll(_pollfds.data(), _pollfds.size(), -1) == -1)		//Check file descriptors
		{
			std::cerr << "Error polling sockets " << strerror(errno) << std::endl;
			break;
		}
		checkServerSocket();

		checkClientSockets(clientBuffers);
	}

	closeFDs();

	std::cout << "Server stopped" << std::endl;
	std::cout << "Goodbye" << std::endl;	
}

time_t stringToUnixTimeStamp(std::string time)
{
	struct tm tm;
	if (strptime(time.c_str(), "%a %b %d %H:%M:%S %Y", &tm) == NULL)
	{
		std::cout << "Error: strptime" << std::endl;
		return (-1);
	}
	return (mktime(&tm));
}

std::string getCurrentTime()
{
	time_t now = time(0);
	char timeStr[100];
    struct tm *localTime = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "%a %b %d %H:%M:%S %Y", localTime);
    return std::string(timeStr);
}

std::string getCurrentEpochTime()
{
    time_t now = time(0);
    return std::to_string(now);
}

/*
	Uses the client's nickname to find and return its socket.
*/
int		Server::getClientSocket(std::string nickname)
{
	int socket;

	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (nickname == _clients[i].getNickname())
		{
			socket = _clients[i].getSocket();
			return (socket);
		}
	}
	return (-2);//Return -2 to differentiate from -1 (failed socket)
}

