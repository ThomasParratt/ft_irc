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
			// std::string notice = user + " :" + message + " " + channel + " \r\n";
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

int		Server::channelExists(std::string channel)
{
	for (auto &it : channel_names)
	{
		if (it.name == channel)
			return (1);
	}
	return (0);
}

int		Server::kickCommand(Msg msg, int clientSocket, Client &client)
{
	//std::cout << "KICK COMMAND" << std::endl;
	if (channelExists(msg.parameters[0]))
	{
		//std::cout << "CHANNEL EXISTS" << std::endl;
		if (userExists(client.getNickname(), msg.parameters[0]))
		{
			//std::cout << "USER KICKING EXISTS ON CHANNEL" << std::endl;
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
									//std::cout << "KICK" << std::endl;
									int i = getChannelIndex(msg.parameters[0], channel_names);
									std::string kick = ":" + kicker.nickname + " KICK " + channel.name + " " + msg.parameters[1] + "\r\n";
									broadcastToChannel(channel_names[i], kick);
									removeUser(msg.parameters[1], msg.parameters[0], "You have been kicked from");
								}
								else
								{
									//std::cout << "KICKEE DOESN'T EXIST" << std::endl;
									std::string notice = ":ircserv NOTICE " + channel.name + " :" + msg.parameters[1] + " is not on this channel\r\n";
									send(clientSocket, notice.c_str(), notice.size(), 0);
									// std::string priv = ":ircserv PRIVMSG " + channel.name + " :" + msg.parameters[1] + " is not on this channel\r\n";
									// send(clientSocket, priv.c_str(), priv.size(), 0);
									// std::string message_441 = ":ircserv 441 " + client.getNickname() + " " + msg.parameters[0] + " " + msg.parameters[1] + " :They aren't on that channel\r\n";
									// send(clientSocket, message_441.c_str(), message_441.size(), 0); //THIS WILL EXIT THE CHANNEL WINDOW
								}
							}
							else
							{
								//std::cout << "USER IS NOT AN OPERATOR" << std::endl;
								std::string notice = ":ircserv NOTICE " + client.getNickname() + " :You're not channel operator for " + channel.name + "\r\n";
								send(clientSocket, notice.c_str(), notice.size(), 0);
								std::string priv = ":ircserv PRIVMSG " + channel.name + " :You're not channel operator\r\n";
								send(clientSocket, priv.c_str(), priv.size(), 0);
								// std::string message_482 = ":ircserv 482 " + client.getNickname() + " " + channel.name + " :You're not channel operator\r\n";
								// send(clientSocket, message_482.c_str(), message_482.size(), 0); //THIS WILL EXIT THE CHANNEL WINDOW
							}
						}
					}
				}
			}
		}
		else
		{
			//std::cout << "USER DOESN'T EXIST ON CHANNEL" << std::endl;
			std::string message_442 = ":ircserv 442 " + client.getNickname() + " " + msg.parameters[0] + " :You're not on that channel\r\n";
			send(clientSocket, message_442.c_str(), message_442.size(), 0);
		}
	}
	else
	{
		//std::cout << "CHANNEL DOESN'T EXIST" << std::endl;
		std::string message_403 = ":ircserv 403 " + client.getNickname() + " " + msg.parameters[0] + " :No such channel\r\n";
		send(clientSocket, message_403.c_str(), message_403.size(), 0);
	}
	return (0);
}