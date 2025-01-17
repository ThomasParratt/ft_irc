#include "Server.hpp"

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

/*
	Initializes Server
	i. creates server socket
	ii. configures used fds to be reuseable
	iii. Defines server address
	iv. binds server socket
	v. starts listening
	vi. sets other minor parameters
*/
int Server::serverInit() 
{
	// _serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // SOCK_NONBLOCK is not supported on macOS
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
	{
		std::cerr << "Error creating socket" << std::endl;
		return (-1);
	}

	int opt = 1;
	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // Set the SO_REUSEADDR option

	// Define the server address
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(_port);
	_serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(_serverSocket, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr)) == -1)
	{
		std::cerr << "Error binding socket" << std::endl;
		return (-2);
	}
	if (listen(_serverSocket, 5) == -1)
	{
		std::cerr << "Error listening on socket" << std::endl;
		return (-3);
	}
	if (setServHostName() == -1)
	{
		std::cerr << "Error setting server hostname" << std::endl;
		return (-4);
	}

	_startTime = time(nullptr); 											// saves the Server starting time
	_startTimeStr = getCurrentTime();
	std::cout << "Server started at " << _startTimeStr << std::endl;

	servRunning = true;														// set servRunning to true (global variable)

	return (1); //Success
}
