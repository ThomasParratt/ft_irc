#include "Server.hpp"

/*
	Sends message to everyone in Channel (except sender).
*/
void	Server::userMessageToChannel(Channel channel, int sender_socket, std::string message)
{
	std::vector<User>	users;
	int					socket;

	users = channel.getChannelUsers();
	// printChannels();
	for (size_t i = 0; i < users.size(); i++)
	{
		socket = getClientSocket(users[i].nickname);
		if (socket != -2 && socket != sender_socket)
		{
			send(socket, message.c_str(), message.size(), 0);
			LOG_SERVER(message);
		}
	}
}

void		Server::channelMessage(Msg msg, Client &client)
{
	int i = getChannelIndex(msg.parameters[0], _channel_names);
	if (i != -1)
	{
		std::string message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + this->_servHostName + " " + msg.command + " " + msg.parameters[0] + " "  + ":" +  msg.trailing_msg + "\r\n";
		userMessageToChannel(_channel_names[i], client.getSocket(), message);			
	}
	else
	{
		// 17_1 so what should we do there? in what scenario would the channel not exist?
		std::cout << "Channel to send message to not found" << std::endl;	//A thought --- maybe we might want a message for a case where Channel doesn't exist.
	}
}

/*
	Sends direct message to specified user.
*/
void		Server::directMessage(Msg msg, Client &client)
{
	std::string other_client_user_name = msg.parameters[0];

	int other_client_socket = getClientSocket(other_client_user_name);
	if (other_client_socket == -2)
	{
		std::string message = "ircserv: Nickname that you wish to DM Doesn't exist.\r\n";//TODO: This should be redirected to the DM window.
		//14:49 -!- mananananananana: No such nick/channel 
		//:server.name 401 <your_nickname> <target> :No such nick/channel [from gpt]
		send(client.getSocket(), message.c_str(), message.size(), 0);
		LOG_SERVER(message);
	}
	else
	{
		std::string message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + this->_servHostName + " " + msg.command + " " + msg.parameters[0] + " "  + ":" +  msg.trailing_msg + "\r\n";
		send(other_client_socket, message.c_str(), message.size(), 0);
		LOG_SERVER(message);
	}
}
/*
	Sends message to either Channel or as a Direct Message to user.
*/
void		Server::privmsgCommand(Msg msg, Client &client)
{
	if (msg.parameters[0][0] == '#')	//if Channel
	{
		std::string channel_name = msg.parameters[0];

		if (userExists(client.getNickname(), channel_name))		
			channelMessage(msg, client);
		else
			std::cout << "User is not on channel" << std::endl;
	}
	else
	{
		directMessage(msg, client);
	}
	return ;
}
