#include "Server.hpp"
#include "Msg.hpp"

int		Server::removeUser(std::string user, std::string channel, std::string message, int partOrKick)
{
	int i = getChannelIndex(channel, _channel_names);
	for (size_t j = 0 ; j < sizeof(_channel_names[i].getChannelUsers()) ; j++)
	{
		//if (channel_names[i].channel_users[j].nickname == user)
		if (_channel_names[i].getChannelUserStruct(j).nickname == user)
		{
			int socket = getClientSocket(_channel_names[i].getChannelUserStruct(j).nickname);
			//channel_names[i].channel_users.erase(channel_names[i].channel_users.begin() + j);
			_channel_names[i].removeUserFromChannelUsers(j);
			// channel_names[i].channel_users.erase(channel_names[i].channel_users.begin() + j);
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

void		Server::kickCommand(Msg msg, Client &client) 
{
	if (channelExists(msg.parameters[0]))
	{
		if (userExists(client.getNickname(), msg.parameters[0]))
		{
			for (auto &channel : _channel_names)
			{
				if (channel.getChannelName() == msg.parameters[0])
				{
					for (auto &kicker : channel.getChannelUsers())
					{
						if ((kicker.nickname == client.getNickname()))
						{
							if (kicker.operator_permissions)
							{
								if (userExists(msg.parameters[1], msg.parameters[0]))
								{
									int i = getChannelIndex(msg.parameters[0], _channel_names);
									std::string kick = ":" + kicker.nickname + " KICK " + channel.getChannelName() + " " + msg.parameters[1] + " " + msg.trailing_msg + "\r\n";
									broadcastToChannel(_channel_names[i], kick, client, 0);
									removeUser(msg.parameters[1], msg.parameters[0], "You have been kicked from", 1);
									client.leaveChannel(msg.parameters[0]);
									// if (!channel.invited.empty())//channel.getInvitedList()
									if (sizeof(channel.getInvitedList()) > 0)
									{
										for (size_t i = 0; i < sizeof(channel.getInvitedList()); i++)
										{
											if (msg.parameters[1] == channel.getInvitedName(i))
											{
												channel.uninviteUser(i);
												break ;
											}
										}
									}
								}
								else
								{
									std::string message_441 = ":ircserv 441 " + client.getNickname() + " " + msg.parameters[0] + " " + msg.parameters[1] + " :They aren't on that channel\r\n";
									send(client.getSocket(), message_441.c_str(), message_441.size(), 0);
								}
							}
							else
							{
								std::string message_482 = ":ircserv 482 " + client.getNickname() + " " + channel.getChannelName() + " :You're not a channel operator\r\n";
								send(client.getSocket(), message_482.c_str(), message_482.size(), 0);
							}
						}
					}
				}
			}
		}
		else
		{
			std::string message_442 = ":ircserv 442 " + client.getNickname() + " " + msg.parameters[0] + " :You're not on that channel\r\n";
			send(client.getSocket(), message_442.c_str(), message_442.size(), 0);
		}
	}
	else
	{
		std::string message_403 = ":ircserv 403 " + client.getNickname() + " " + msg.parameters[0] + " :No such channel\r\n";
		send(client.getSocket(), message_403.c_str(), message_403.size(), 0);
	}
}