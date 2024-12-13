#include "Server.hpp"
#include "Msg.hpp"

// removes a user from a channel
int		Server::removeUser(std::string user, std::string channel, std::string message)
{
	int i = getChannelIndex(channel, channel_names);
	for (int j = 0 ; j < sizeof(channel_names[i].channel_users) ; j++)
	{
		if (channel_names[i].channel_users[j].nickname == user)
		{
			int socket = getClientSocket(channel_names[i].channel_users[j].nickname);
			channel_names[i].channel_users.erase(channel_names[i].channel_users.begin() + j);
			std::string notice = ":ircserv NOTICE " + user + " :" + message + " " + channel + " \r\n";
			send(socket, notice.c_str(), notice.size(), 0);
			return (1);
		}
	}
	return (0);
}

// checks if a user exists in a channel
int		Server::userExists(std::string user, std::string channel)
{
	int i = getChannelIndex(channel, channel_names);
	for (auto &userToCheck : channel_names[i].channel_users)
	{
		if (userToCheck.nickname == user)
			return (1);
	}
	return (0);
}

int		Server::inviteCommand(Msg msg, int clientSocket, Client &client)
{
	return (0);
}

int		Server::kickCommand(Msg msg, int clientSocket, Client &client)
{
	for (auto &channel : channel_names)
	{
		if (channel.name == msg.parameters[0]) 
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
							std::string kick = ":" + kicker.nickname + " KICK " + channel.name + " " + msg.parameters[1] + "\r\n";
							broadcastToChannel(channel_names[i], kick);
							//printChannels();
							removeUser(msg.parameters[1], msg.parameters[0], "You have been kicked from");
							//printChannels();
							return (0);
						}
						else
						{
							std::string notice = ":ircserv NOTICE " + channel.name + " :" + msg.parameters[1] + " is not on this channel\r\n";
							send(clientSocket, notice.c_str(), notice.size(), 0);
							return (0);
						}
					}
					else
					{
						// NO OPERATOR PERMISSIONS
						std::string notice = ":ircserv NOTICE " + channel.name + " :You're not a channel operator\r\n";
						send(clientSocket, notice.c_str(), notice.size(), 0);
						return (0);
					}
				}
			}
		}
	}
	return (0);
}