
#include "Server.hpp"

/*
	1. Makes a new channel user with...
		i. Nickname
		ii. Permissions (operator (= true/false))
	2. Adds user into vector of channel users
*/
void	Server::addChannelUser(Channel &channel, Client &client, bool operator_permissions)
{
	//Make new Channel User
	User	new_user;
	new_user.nickname				= client.getNickname();
	new_user.operator_permissions	= operator_permissions;

	//Add into Channel Users
	std::vector<User> channel_users;

	channel_users = channel.channel_users;
	channel_users.push_back(new_user);
	client.joinChannel(channel.name);

	// channel_users = channel.getChannelUsers();
	// channel_users.push_back(new_user);
	// client.joinChannel(channel.getChannelName());
}

int		Server::createChannel(Msg msg, int clientSocket, Client &client)
{
	std::string 	name = msg.parameters[0];

	//Make new Channel
	Channel			new_channel(name);

	//Add User into Channel
	addChannelUser(new_channel, client, true);

	//Update Channel vector
	this->channel_names.push_back(new_channel);

	std::string response = ":ircserver PRIVMSG " + msg.parameters[0] + " :Welcome to the Channel " + msg.parameters[0] + "\r\n";	

	send(clientSocket, response.c_str(), response.size(), 0);

	/*
		TODO: Send more messages to Irssi f.ex. tell Irssi who is operator -> Check Manual
	*/

	return (0);
}

int		Server::joinChannel(Msg msg, int clientSocket, Client &client, int index)
{

	if (channelJoinChecks(this -> channel_names[index], msg, clientSocket, client) != 0)
	{
		return (1);
	}
	addChannelUser(this->channel_names[index], client, false);

	return (0);
}

int		Server::joinCommand(Msg msg, int clientSocket, Client &client)
{
	int i = getChannelIndex(msg.parameters[0], this->channel_names);

	if (i == -1)	//No channel found
	{
		// std::cout << "No Existing Channel Found" << std::endl;
		createChannel(msg, clientSocket, client);
	}
	else			//Channel found
	{ 
		// std::cout << "Channel Found at i = " << i << std::endl;
		if (joinChannel(msg, clientSocket, client, i) != 0)// If fail - then leave???
			return (1);
		
	}
	i = getChannelIndex(msg.parameters[0], this->channel_names);

	// need to make sure that we don't send this message to the channels if the user didn't join
	std::string message = ":ircserver PRIVMSG " + msg.parameters[0] + " :" + client.getNickname() + " has joined " + msg.parameters[0] + "\r\n";	
	broadcastToChannel(this->channel_names[i], message);//Send sender Fd??

	//WELCOME_MSG - Send message to client who connected to channel

	/*
		Send This to Server!
			:sender_nickname!user@host PRIVMSG #channel_name :message_text
			:Alice!alice@irc.example.com PRIVMSG #general :Hello everyone!
	*/

	printChannels();
	return (0);
}

