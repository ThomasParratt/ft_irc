#include "Server.hpp"
#include "Msg.hpp"

void	Server::inviteCommand(Msg msg, Client &client)
{
	if (channelExists(msg.parameters[1]))
	{
		if (userExists(client.getNickname(), msg.parameters[1]))
		{
			for (auto &channel : _channel_names)
			{
				for (auto &inviter : channel.getChannelUsers())
				{
					if ((inviter.nickname == client.getNickname()))
					{
						if (inviter.operator_permissions)
						{
							if (!userExists(msg.parameters[0], msg.parameters[1]))
							{
								std::string message_341 = ":ircserv 341 " + client.getNickname() + " " + msg.parameters[0] + " " + msg.parameters[1] + "\r\n";
								int socket = getClientSocket(msg.parameters[0]);
								send(socket, message_341.c_str(), message_341.size(), 0);
								LOG_SERVER(message_341);
								channel.addUserToInviteList(msg.parameters[0]);
							}
							else
							{
								std::string message_443 = ":ircserv 443 " + client.getNickname() + " " + msg.parameters[0] + " " + msg.parameters[1] + " :is already on channel\r\n";
								send(client.getSocket(), message_443.c_str(), message_443.size(), 0);
								LOG_SERVER(message_443);
							}
						}
						else
						{
							std::string message_482 = ":ircserv 482 " + client.getNickname() + " " + msg.parameters[1] + " :You're not a channel operator\r\n";
							send(client.getSocket(), message_482.c_str(), message_482.size(), 0);
							LOG_SERVER(message_482);
						}
					}
				}
			}
		}
		else
		{
			std::string message_442 = ":ircserv 442 " + client.getNickname() + " " + msg.parameters[1] + " :You're not on that channel\r\n";
			send(client.getSocket(), message_442.c_str(), message_442.size(), 0);
			LOG_SERVER(message_442);
		}
	}
	else
	{
		std::string message_403 = ":ircserv 403 " + client.getNickname() + " " + msg.parameters[1] + " :No such channel\r\n";
		send(client.getSocket(), message_403.c_str(), message_403.size(), 0);
		LOG_SERVER(message_403);
	}
}