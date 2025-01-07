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

// checks if a user exists in a channel
int		Server::userExists(std::string user, std::string channel)
{
	int i = getChannelIndex(channel, channel_names);
	for (auto &userToCheck : channel_names[i].getChannelUsers())
	{
		if (userToCheck.nickname == user)
			return (1);
	}
	return (0);
}

int		Server::channelExists(std::string channel)
{
	for (auto &it : channel_names)
	{
		if (it.name == channel)
			return (1);
	}
	return (0);
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
		if (channel_name == channel_names[i].getChannelName())
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
