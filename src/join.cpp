
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
	this->_channel_names.push_back(new_channel);

	std::string message;
	message = ":ircserver 353 " + client.getNickname() + " @ " + msg.parameters[0] + " " + ": @" + client.getNickname() + "\r\n";
	send(clientSocket, message.c_str(), message.size(), 0);

	std::string message1;
	message1 = ":ircserver 366 " + client.getPrefix() + " " + msg.parameters[0] + " :End of /NAMES list" + "\r\n";//
	send(clientSocket, message1.c_str(), message1.size(), 0);

	std::string channelCreated;
	channelCreated = ":ircserver 329 " + client.getNickname() + " " + msg.parameters[0] + " " + getCurrentTime() + "\r\n";
	send(client.getSocket(), channelCreated.c_str(), channelCreated.size(), 0);

	/*
		TODO: Send more messages to Irssi f.ex. tell Irssi who is operator -> Check Manual
	*/

	return (0);
}

int		Server::joinChannel(Msg msg, int clientSocket, Client &client, int index)
{

	if (channelJoinChecks(this ->_channel_names[index], msg, clientSocket, client) != 0)
	{
		return (1);
	}
	addChannelUser(this->_channel_names[index], client, false);

	return (0);
}

void Server::joinChannelMessage(std::string channelName, Client &client)
{
	int i = getChannelIndex(channelName, this->_channel_names);

	std::string topic = this->_channel_names[i].getChannelTopic();
	if (!topic.empty())
	{
		topicPrint(channelName, client.getSocket(), client);
	}

	//this needs to be finished
	std::string result;
    const std::vector<User>& channel_users = _channel_names[i].getChannelUsers();  // Using the getter to access the users list

    for (size_t i = 0; i < channel_users.size(); ++i) 
	{
		if (channel_users[i].operator_permissions)
			result += "@" + channel_users[i].nickname;
		else
			result += channel_users[i].nickname;
	
		if (i != channel_users.size() - 1) 
			result += " ";  // Add a space between names besides the last one
    }
    
	std::string message;
	std::cout << "353 result: " << result << std::endl;
	message = ":ircserver 353 " + client.getNickname() + " @ " + channelName + " :" + result + "\r\n";
	send(client.getSocket(), message.c_str(), message.size(), 0);

	std::string message1;
	message1 = ":ircserver 366 " + client.getPrefix() + " " + channelName + " :End of /NAMES list" + "\r\n";//
	send(client.getSocket(), message1.c_str(), message1.size(), 0);

	std::string channelCreated;
	channelCreated = ":ircserver 329 " + client.getNickname() + " " + channelName + " " + this->_channel_names[i].getChannelTime() + "\r\n";
	send(client.getSocket(), channelCreated.c_str(), channelCreated.size(), 0);
}
int		Server::joinCommand(Msg msg, int clientSocket, Client &client)
{
	int i = getChannelIndex(msg.parameters[0], this->_channel_names);

	bool welcomeSent = false;
	if (i == -1)	//No channel found
	{
		// std::cout << "No Existing Channel Found" << std::endl;
		createChannel(msg, clientSocket, client);
		welcomeSent = true;
	}
	else			//Channel found
	{ 
		// std::cout << "Channel Found at i = " << i << std::endl;
		if (joinChannel(msg, clientSocket, client, i) != 0)// If fail - then leave???
			return (1);
		
	}
	i = getChannelIndex(msg.parameters[0], this->_channel_names);
	// need to make sure that we don't send this message to the channels if the user didn't join
	// std::string message = ":ircserver PRIVMSG " + msg.parameters[0] + " :" + client.getNickname() + " [~" + client.getUsername() + "@" + client.getHostIP() + "] has joined " + msg.parameters[0] + "\r\n";

	std::string message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostIP() + " JOIN " + msg.parameters[0] + "\r\n";
	broadcastToChannel(this->_channel_names[i], message, client, 0);
	
	if (welcomeSent == false)
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

