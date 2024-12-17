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

	std::string response = ":ircserver PRIVMSG " + msg.parameters[0] + " :Welcome to the Channel " + msg.parameters[0] + "\r\n";	


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

/*
	Returns index of channel in vector.
	- Returns 0 or greater, if channel exists.
	- Returns -1, if channel doesn't exist. 
*/
int		getChannelIndex(std::string channel_name, std::vector<Channel> channel_names)
{
	int i;

	for (i = 0; i < channel_names.size(); i++)
	{
		if (channel_name == channel_names[i].name)
		{
			return (i);
		}
	}
	return (-1);
}

int		Channel::getNumberOfChannelUsers()
{
	std::vector<User> channel_users = this -> getChannelUsers();

	int num_of_channel_users  = channel_users.size();

	return (num_of_channel_users);
}
/*
	Returns true, if Number of Channel Users equals or exceeds Channel User Limit.
	Returns false,
		i if that is not the case
		ii. if no Channel Limit set.
*/
bool		Channel::isChannelFull()
{
	int user_limit 				= this -> getUserLimit();
	int	num_of_channel_users 	= this -> getNumberOfChannelUsers();

	std::cout << "User_limit: " << user_limit << std::endl;
	std::cout << "Num_of_channel_users: " << num_of_channel_users << std::endl;

	if (user_limit == -1 || user_limit > num_of_channel_users)
	{
		return (false);
	}
	else
	{
		return (true);
	}
}

bool		Channel::doesChannelHavePassword()
{
	if (channel_key == "")
	{
		return (false);
	}
	else
	{
		return (true);
	}
}

int		Server::channelJoinChecks(Channel channel, Msg msg, int clientSocket, Client &client)
{
	std::string message;
	
	if (channel.isChannelFull() == true)
	{
		//channel is full
		message = ":ircserv 471 " + client.getNickname() +  " " + msg.parameters[0] + " :Cannot join channel (+l) - channel is full, try again later\r\n";
		
		//":ft_irc 471 mkorpela_ #ABC567 :Cannot join channel (+l) - channel is full, try again later";
		
		send(clientSocket, message.c_str(), message.size(), 0);

		//Example - 14:13 -!- Cannot join to channel #ABC456 (Channel is full)
		//Example - :copper.libera.chat 471 mkorpela_ #ABC567 :Cannot join channel (+l) - channel is full, try again later		
		return (1) ;
	}
	// channel.invite_only = true;
	// std::cout << "channel.isChannelInviteOnly(): " << channel.isChannelInviteOnly() << std::endl;
	if (channel.isChannelInviteOnly() == true)
	{
		/*
			a. If Invited
				-> OK

			b. If not Invited
				-> Send message
		*/

		// YOU SHOULD'VE BEEN INVITED. 
		message  = ":ircserv 473 " + client.getNickname() + " " + msg.parameters[0] + " :Cannot join channel (+i) - you must be invited\r\n";
		send(clientSocket, message.c_str(), message.size(), 0);

		//example << JOIN #BANNNANAss
		//example >> :lithium.libera.chat 473 gravity123 #BANNNANAss :Cannot join channel (+i) - you must be invited
		return (1);
	}
	
 	if (channel.doesChannelHavePassword() == true)
	{
		if (msg.parameters.size() <= 1)						 //No password parameter passed.
		{
			message = ":ircserv 475 " + client.getNickname() + " " + msg.parameters[0] + " :Cannot join channel (+k) - no password entered\r\n"; // Trailing messages don't print
			send(clientSocket, message.c_str(), message.size(), 0);

			// << JOIN #ABC1234
			// >> :molybdenum.libera.chat 475 apppleess #ABC1234 :Cannot join channel (+k) - bad key

			return (1);
		}
		if (channel.getChannelKey() == msg.parameters[1]) 	// Password Correct
		{

			// message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + this->getServHostName() + " JOIN " + msg.parameters[0] + "\r\n";
			// std::cout << "message: " << message;
			// send(clientSocket, message.c_str(), message.size(), 0); //Send MESSAGE here if needed... I'll think about it later. :)

			// << JOIN #ABC1234 123456
			// >> :apppleess!~mkorpela@194.136.126.52 JOIN #ABC1234
		}
		else 												//Password Incorrect
		{
			message = ":ircserv 475 " + client.getNickname() + " " + msg.parameters[0] + " :Cannot join channel (+k) - password incorrect\r\n"; // Trailing messages don't print
			send(clientSocket, message.c_str(), message.size(), 0);

			// << JOIN #ABC1234 lol
			// >> :molybdenum.libera.chat 475 apppleess #ABC1234 :Cannot join channel (+k) - bad key

			return (1);
		}
	}
	return (0);
}

int		Server::joinChannel(Msg msg, int clientSocket, Client &client)
{
	std::cout << "in Join Channel" << std::endl;

	int index = getChannelIndex(msg.parameters[0], this->channel_names);
	if (index == -1)
	{
		//Channel doesn't exist
	}

	/*
		1. Check user_limit
			-1
				-> all good
			(user_limit <= number_of_users_in_channel)
			f.ex. 20 <= 21


			if user_limit >= getNumberOfChannelUsers()
			getNumberOfChannelUsers

		2. Check invite_only
		3. Check Password
	*/

	if (channelJoinChecks(this -> channel_names[index], msg, clientSocket, client) != 0)
	{
		return (1);
	}

	addChannelUser(this->channel_names[index], client, false); // THIS comes later..





	//TODO: Do we send a message to Irssi? send broadcast msg?

	//Broadcast to All NEW USER

	return (0);
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

void	Server::broadcastToChannel(Channel &channel, std::string message)
{
	std::vector<User> users;
	int socket;
	// std::cout << "in broadcastToChannel Function" << std::endl;	
	users = channel.getChannelUsers();

	for (int i = 0; i < users.size(); i++)
	{
		// users[i].nickname;
		int socket = getClientSocket(users[i].nickname);
		if (socket != -2)
		{
			send(socket, message.c_str(), message.size(), 0);
		}
	}
}