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
	pollfds.push_back(clientPollfd);
	clients.emplace_back(clientSocket, _password);
	std::cout << "New client connected, socket " << clientSocket << std::endl;
}

void Server::serverLoop() {
	// set serverPollfd
	pollfd serverPollfd;
	serverPollfd.fd = _serverSocket;
	serverPollfd.events = POLLIN | POLLERR;
	// push serverPollfd to pollfds
	pollfds.push_back(serverPollfd);
	std::cout << "Server loop started" << std::endl;
	while (servRunning) {
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
				if (bytesRead <= 0) {
					if (bytesRead == 0) 
						std::cout << "Client disconnected, socket " << pollfds[i].fd << std::endl;
					else 
						std::cerr << "Error reading from socket " << pollfds[i].fd << strerror(errno) << std::endl;
					close(pollfds[i].fd);
					pollfds.erase(pollfds.begin() + i);
					clients.erase(clients.begin() + (i - 1)); //changed this
					i--;
					continue;
				}
				std::cout << "Message received from socket " << pollfds[i].fd << std::endl << buffer;
				for (auto &client : clients) //changed this
                {
                    if (client.getSocket() == pollfds[i].fd) 
                    {
                        if (this->messageHandler(buffer, pollfds[i].fd, client) == 1)
						{
							std::cout << "Client disconnected, socket " << pollfds[i].fd << std::endl;
							close(pollfds[i].fd);
							pollfds.erase(pollfds.begin() + i);
							clients.erase(clients.begin() + (i - 1)); //changed this
							i--;
							continue;
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

/* updateChannelMap we can intrduce a third parameter that checks if its join or quit | 0 for join and 1 for quit*/
void Server::updateChannelMap(std::string channelName, Client *client, int joinOrQuit)
{
	if (joinOrQuit == 0)
	{
		auto it = channel_map.find(channelName);
		if (it == channel_map.end())
		{
			channel_map[channelName] = std::vector<Client*>();
		}
		channel_map[channelName].push_back(client);
		//broadcastmessage
	}
	else
	{
		auto it = channel_map.find(channelName);
		if (it != channel_map.end())
		{
			for (auto it = channel_map[channelName].begin(); it != channel_map[channelName].end(); it++)
			{
				if (*it == client)
				{
					//broadcastmessage
					channel_map[channelName].erase(it);
					break;
				}
			}
		}
		if (channel_map[channelName].empty())
		{
			channel_map.erase(channelName);
		}
	}
}

void Server::broadcastMessage(const std::string& channelName, const std::string& message, int senderSocket) 
{
    auto it = channel_map.find(channelName);
    if (it != channel_map.end()) {
        for (Client* client : it->second) {
            if (client->getSocket() != senderSocket) {
                send(client->getSocket(), message.c_str(), message.size(), 0);
            }
        }
    }
}

// std::string messageParam(char *buffer, std::string message)
// {
//     std::string param;
//     std::string received(buffer);
//     size_t startPos = received.find(message);
//     if (startPos != std::string::npos)
//     {
//         size_t endPos = received.find("\n", startPos);
//         if (endPos != std::string::npos)
//             param = received.substr(startPos + 5, endPos - (startPos + 6));
//     }
//     return (param);
// }

// int    handleMessages(char *buffer, int clientSocket, Client &client)
// {
//     std::cout << "Handling messages" << std::endl;
//     std::string clientPassword = messageParam(buffer, "PASS ");
//     if (!clientPassword.empty())
//     {
//         if (clientPassword != client.getPassword())
//         {
//             send(clientSocket, "Error: Wrong password\r\n", 23, 0);
//             return (1);
//         }
//     }
//     client.setNickname(messageParam(buffer, "NICK "));
//     if (!client.getNickname().empty())
//     {
//         std::string response = "You are now known as " + client.getNickname() + "\r\n";
//         send(clientSocket, response.c_str(), response.size(), 0);
//         if (!client.getWelcomeSent())
//         {
//             std::string message_001 = ":ircserv 001 " + client.getNickname() + " :Welcome to the IRC network " + client.getNickname() + "\r\n";
//             send(clientSocket, message_001.c_str(), message_001.size(), 0);
//             std::string message_002 = ":ircserv 002 " + client.getNickname() + " :Your host localhost, running version ircserv1.0\r\n";
//             send(clientSocket, message_002.c_str(), message_002.size(), 0);
//             return (2);
//         }
//     }
//     std::string ping = messageParam(buffer, "PING ");
//     if (!ping.empty())
//     {
//         std::string response = "PONG " + ping + "\r\n";
//         send(clientSocket, response.c_str(), response.size(), 0);
//     }
//     return (0);
// }

std::string getCurrentTime()
{
	time_t now = time(0);
	char timeStr[100];
    struct tm *localTime = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTime);
    return std::string(timeStr);
}

void	Server::addChannelUser(Channel &channel, Client &client, bool operator_permissions)
{
	//Make new Channel User
	User	new_user;
	new_user.nickname = client.getNickname();
	new_user.operator_permissions = operator_permissions;

	//Add into channel Users
	channel.channel_users.push_back(new_user);
}

int		Server::createChannel(Msg msg, int clientSocket, Client &client)
{
	Channel new_channel(msg.parameters[0]);

	addChannelUser(new_channel, client, true);

	this->channel_names.push_back(new_channel);


	std::string response = ":" + client.getNickname() + "!" + "USERNAME@ircserver PRIVMSG " + msg.parameters[0] + " :Welcome to the Channel " + msg.parameters[0] + "\r\n";
	
	send(clientSocket, response.c_str(), response.size(), 0);

	/*
		TODO: Send more messages to Irssi f.ex. tell Irssi who is operator -> Check Manual
	*/


	return (0);
}

void	Server::printChannelUsers(Channel channel)
{
	for (int i = 0; i < channel.channel_users.size(); i++)
	{
		std::cout << "User[" << i << "]: " <<  channel.channel_users[i].nickname  << " (op = " <<  channel.channel_users[i].operator_permissions << ")" << std::endl;
	}	
}

void		Server::printChannels()
{
	for (int i = 0; i < this->channel_names.size(); i++)
	{
		std::cout << "Channel[" << i << "]: "<< channel_names[i].name << std::endl;
		printChannelUsers(channel_names[i]);
	}
}

int		Server::joinChannel(Msg msg, int clientSocket, Client &client)
{
	int index = getChannelIndex(msg.parameters[0], this->channel_names);

	addChannelUser(this->channel_names[index], client, false);

	//TODO: Do we send a message to Irssi? send broadcast msg?

	//Broadcast to All NEW USER

	return (0);
}
