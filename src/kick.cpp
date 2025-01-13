#include "Server.hpp"
#include "Msg.hpp"

int		Server::removeUser(std::string user, std::string channel, std::string message, int partOrKick)
{
	int i = getChannelIndex(channel, channel_names);
	for (int j = 0 ; j < sizeof(channel_names[i].channel_users) ; j++)
	{
		if (channel_names[i].channel_users[j].nickname == user)
		{
			int socket = getClientSocket(channel_names[i].channel_users[j].nickname);
			channel_names[i].channel_users.erase(channel_names[i].channel_users.begin() + j);
			if (partOrKick == 1) //kick
			{
				std::string notice = ":ircserv NOTICE " + user + " :" + message + " " + channel + " \r\n";
				send(socket, notice.c_str(), notice.size(), 0);
			} 
			else if (partOrKick == 0) //part
			{
				send(socket, message.c_str(), message.size(), 0);
				std::cout << "Debug: message = " << message << std::endl;
			}
			else
				std::cout << "Debug: message = " << message << std::endl;
			return (1);
		}
	}
	return (0);
}

int		Server::kickCommand(Msg msg, int clientSocket, Client &client) 
{
	if (channelExists(msg.parameters[0]))
	{
		if (userExists(client.getNickname(), msg.parameters[0]))
		{
			for (auto &channel : channel_names)
			{
				if (channel.getChannelName() == msg.parameters[0])
				{
					for (auto &kicker : channel.channel_users)
					{
						if ((kicker.nickname == client.getNickname()))
						{
							if (kicker.operator_permissions)
							{
								if (userExists(msg.parameters[1], msg.parameters[0]))
								{
									int i = getChannelIndex(msg.parameters[0], channel_names);
									std::string kick = ":" + kicker.nickname + " KICK " + channel.getChannelName() + " " + msg.parameters[1] + " " + msg.trailing_msg + "\r\n";
									broadcastToChannel(channel_names[i], kick);
									removeUser(msg.parameters[1], msg.parameters[0], "You have been kicked from", 1);
									client.leaveChannel(msg.parameters[0]);
								}
								else
								{
									std::string message_441 = ":ircserv 441 " + client.getNickname() + " " + msg.parameters[0] + " " + msg.parameters[1] + " :They aren't on that channel\r\n";
									send(clientSocket, message_441.c_str(), message_441.size(), 0);
								}
							}
							else
							{
								std::string message_482 = ":ircserv 482 " + client.getNickname() + " " + channel.name + " :You're not a channel operator\r\n";
								send(clientSocket, message_482.c_str(), message_482.size(), 0);
							}
						}
					}
				}
			}
		}
		else
		{
			std::string message_442 = ":ircserv 442 " + client.getNickname() + " " + msg.parameters[0] + " :You're not on that channel\r\n";
			send(clientSocket, message_442.c_str(), message_442.size(), 0);
		}
	}
	else
	{
		std::string message_403 = ":ircserv 403 " + client.getNickname() + " " + msg.parameters[0] + " :No such channel\r\n";
		send(clientSocket, message_403.c_str(), message_403.size(), 0);
	}
	return (0);
}