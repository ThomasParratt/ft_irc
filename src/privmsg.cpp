
#include "Server.hpp"

void	Server::userMessageToChannel(Channel channel, int sender_socket, std::string message)
{
	std::vector<User> users;
	int socket;

	users = channel.getChannelUsers();
	// printChannels();
	for (int i = 0; i < users.size(); i++)
	{
		int socket = getClientSocket(users[i].nickname);
		if (socket != -2 && socket != sender_socket)
		{
			send(socket, message.c_str(), message.size(), 0);
		}
	}
}

void		Server::directMessage(Msg msg, int clientSocket, Client &client)
{
	int socket = getClientSocket(msg.parameters[0]);
	if (socket == -2)
	{
		std::cout << "Nickname that you wish to DM Doesn't exist." << std::endl;
	}
	else
	{
		// FORMAT -> :client1!user@host PRIVMSG client2 :message		
		std::string message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + this->_servHostName + " " + msg.command + " " + msg.parameters[0] + " "  + ":" +  msg.trailing_msg + "\r\n";
		send(socket, message.c_str(), message.size(), 0);
	}
}

void		Server::channelMessage(Msg msg, int clientSocket, Client &client)
{
	int i = getChannelIndex(msg.parameters[0], channel_names);
	if (i != -1)
	{
		//FORMAT -> :UserA!user@host PRIVMSG #general :Hello, world!
		std::string message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + this->_servHostName + " " + msg.command + " " + msg.parameters[0] + " "  + ":" +  msg.trailing_msg + "\r\n";
		userMessageToChannel(channel_names[i], clientSocket, message);			
	}
	else
	{
		std::cout << "Channel to send message to not found" << std::endl;
	}
}

int		Server::privmsgCommand(Msg msg, int clientSocket, Client &client)
{
	//ToDo: Check for that parameter exists, if someones sends with rawmsg etc. [Maybe do this...]

	if (msg.parameters[0][0] == '#')
	{
		if (userExists(client.getNickname(), msg.parameters[0]))
			channelMessage(msg, clientSocket, client);
		else
			std::cout << "User is not on channel" << std::endl;
	}
	else
	{
		directMessage(msg, clientSocket, client);
	}
	return (0);
}
