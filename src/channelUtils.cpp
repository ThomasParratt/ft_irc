#include "Server.hpp"

/*
	Debug method
*/
void	Server::printChannelUsers(Channel channel)
{
	for (int i = 0; i < channel.channel_users.size(); i++)
	{
		std::cout << "User[" << i << "]: " <<  channel.channel_users[i].nickname  << " (op = " <<  channel.channel_users[i].operator_permissions << ")" << std::endl;
	}	
}

/*
	Debug method
*/
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
/*
	Sends a message to everyone in a specific channel
*/
void	Server::broadcastToChannel(Channel &channel, std::string message)
{
	std::vector<User> users;
	int socket;

	users = channel.getChannelUsers();

	for (int i = 0; i < users.size(); i++)
	{
		socket = getClientSocket(users[i].nickname);
		if (socket != -2)
		{
			send(socket, message.c_str(), message.size(), 0);
		}
	}
}
