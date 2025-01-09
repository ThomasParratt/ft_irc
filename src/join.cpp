
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
	new_user.nickname = client.getNickname();
	new_user.operator_permissions = operator_permissions;

	//Add into channel Users
	channel.addUserIntoChannelUsers(new_user);
	client.joinChannel(channel.getChannelName());

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
//maybe we can use privmsg to send this message to the client in the channel

void Server::joinChannelMessage(std::string channelName, Client &client)
{
	int i = getChannelIndex(channelName, this->channel_names);
	int OpCount = this->channel_names[i].getOpCount();
	int totalCount = this->channel_names[i].getTotalCount();
	std::string topic = this->channel_names[i].getChannelTopic();
	if (!topic.empty())
	{
		topicPrint(channelName, client.getSocket(), client);
	}
	std::string joinMsg;
	joinMsg = ":ircserver PRIVMSG " + channelName + " :Total of " + std::to_string(totalCount) + " nicks [" + std::to_string(OpCount) + " ops, " + std::to_string(totalCount - OpCount) + " normal]\r\n";
	send(client.getSocket(), joinMsg.c_str(), joinMsg.size(), 0);
	
	std::string channelCreated;
	channelCreated = ":ircserver PRIVMSG " + channelName + " :Channel " + channelName + " create " + this->channel_names[i].getChannelTime() + "\r\n";
	send(client.getSocket(), channelCreated.c_str(), channelCreated.size(), 0);
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
	// std::string message = ":ircserver PRIVMSG " + msg.parameters[0] + " :" + client.getNickname() + " [~" + client.getUsername() + "@" + client.getHostIP() + "] has joined " + msg.parameters[0] + "\r\n";
	std::string message = ":" + client.getNickname() + "!" + "~" + client.getUsername() + "@" + client.getHostIP() + " JOIN " + msg.parameters[0] + "\r\n";
	broadcastToChannel(this->channel_names[i], message);//Send sender Fd??
	joinChannelMessage(msg.parameters[0], client);
	//WELCOME_MSG - Send message to client who connected to channel

	/*
		Send This to Server!
			:sender_nickname!user@host PRIVMSG #channel_name :message_text
			:Alice!alice@irc.example.com PRIVMSG #general :Hello everyone!
	*/

	printChannels();
	return (0);
}

