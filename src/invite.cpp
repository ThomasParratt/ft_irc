#include "Server.hpp"
#include "Msg.hpp"

// /invite nick channel
// invite multiple users in one command?
int		Server::inviteCommand(Msg msg, int clientSocket, Client &client)
{
	//std::cout << "INVITE COMMAND" << std::endl;
	if (channelExists(msg.parameters[1]))
	{
		//std::cout << "CHANNEL EXISTS" << std::endl;
		if (userExists(client.getNickname(), msg.parameters[1]))
		{
			//std::cout << "USER INVITING EXISTS ON CHANNEL" << std::endl;
			for (auto &channel : channel_names)
			{
				for (auto &inviter : channel.channel_users)
				{
					if ((inviter.nickname == client.getNickname()))
					{
						if (inviter.operator_permissions)
						{
							if (!userExists(msg.parameters[0], msg.parameters[1])) // what if the invitee doesn't exist at all? socket failure?
							{
								//std::cout << "INVITE" << std::endl;
								std::string message_341 = ":ircserv 341 " + client.getNickname() + " " + msg.parameters[0] + " " + msg.parameters[1] + "\r\n";
								int socket = getClientSocket(msg.parameters[0]);
								send(socket, message_341.c_str(), message_341.size(), 0);
								// add to invited list
								channel.invited.push_back(msg.parameters[0]);
							}
							else
							{
								//std::cout << "INVITEE ALREADY ON CHANNEL" << std::endl;
								std::string message_443 = ":ircserv 443 " + client.getNickname() + " " + msg.parameters[0] + " " + msg.parameters[1] + " :is already on channel\r\n";
								send(clientSocket, message_443.c_str(), message_443.size(), 0);
							}
						}
						else
						{
							//std::cout << "USER IS NOT AN OPERATOR" << std::endl;
							std::string message_482 = ":ircserv 482 " + client.getNickname() + " " + msg.parameters[1] + " :You're not a channel operator\r\n";
							send(clientSocket, message_482.c_str(), message_482.size(), 0);
						}
					}
				}
			}
		}
		else
		{
			//std::cout << "USER DOESN'T EXIST ON CHANNEL" << std::endl;
			std::string message_442 = ":ircserv 442 " + client.getNickname() + " " + msg.parameters[1] + " :You're not on that channel\r\n";
			send(clientSocket, message_442.c_str(), message_442.size(), 0);
		}
	}
	else
	{
		//std::cout << "CHANNEL DOESN'T EXIST" << std::endl;
		std::string message_403 = ":ircserv 403 " + client.getNickname() + " " + msg.parameters[1] + " :No such channel\r\n";
		send(clientSocket, message_403.c_str(), message_403.size(), 0);
	}
	return (0);
}