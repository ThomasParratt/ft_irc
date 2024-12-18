#include "Server.hpp"
#include "Msg.hpp"

/*
Topic command
checks if enough parameter is given
check if channel exists
check if its asking for the topic or setting the topic
check if user exists in the channel
check if operator privilige is required
*/

int	Server::topicCommand(Msg msg, int clientSocket, Client &client)
{
	printMsg(msg); // debug
	if (msg.parameters[0].empty())
	{
		std::cout << "Not enough parameters" << std::endl;
		std::string error_461 = ":ircserv 461 " + client.getNickname() + " :TOPIC Not enough parameters\r\n";
		send(clientSocket, error_461.c_str(), error_461.size(), 0);
		return 1;
	}
	Channel *tarChannel = getChannel(msg.parameters[0]);
	if (tarChannel == nullptr)
	{
		std::string error_403 = ":ircserv 403 " + client.getNickname() + " " + msg.parameters[0] + " :No such channel\r\n";
		send(clientSocket, error_403.c_str(), error_403.size(), 0);
	}
	else if (msg.trailing_msg.empty())
	{
		if (userExists(client.getNickname(), tarChannel->name))
			topicPrint(tarChannel, clientSocket, client);
		else
		{
			std::string error_442 = ":ircserv 442 " + client.getNickname() + " " + tarChannel->name + " :You're not on that channel\r\n";
			send(clientSocket, error_442.c_str(), error_442.size(), 0);
		}
	}
	else
	{
		std::cout << "Setting Topic" << std::endl;
		if (tarChannel->topic_requires_operator)
		{
			for (auto &setter : tarChannel->channel_users)
			{
				if (setter.nickname == client.getNickname())
				{
					if (!setter.operator_permissions)
					{
						std::string error_482 = ":ircserv 482 " + client.getNickname() + " " + tarChannel->name + " :You're not channel operator\r\n";
						send(clientSocket, error_482.c_str(), error_482.size(), 0);
						return 1;
					}
					else
					{
						tarChannel->setChannelTopic(msg.trailing_msg, client);
						std::string topicMsg = ":" + client.getNickname() + " TOPIC " + tarChannel->name + " :" + msg.trailing_msg + "\r\n";
						broadcastToChannel(*tarChannel, topicMsg);
					}
				}
				else
				{
					std::string error_442 = ":ircserv 442 " + client.getNickname() + " " + tarChannel->name + " :You're not on that channel\r\n";
					send(clientSocket, error_442.c_str(), error_442.size(), 0);
				}
			}
		}
		else
		{
			tarChannel->setChannelTopic(msg.trailing_msg, client);
			std::string topicMsg = ":" + client.getNickname() + " TOPIC " + tarChannel->name + " :" + msg.trailing_msg + "\r\n";
			broadcastToChannel(*tarChannel, topicMsg);
		}
	}
	return 0;
}

void Server::topicPrint(Channel *tarChannel, int clientSocket, Client &client)
{
	if (tarChannel->getChannelTopic().empty())
	{
		std::string noTopicMsg_331 = "No topic is set for " + tarChannel->name + "\r\n";
		send(clientSocket, noTopicMsg_331.c_str(), noTopicMsg_331.size(), 0);
		return;
	}
	else 
	{
		std::string topicMsg = "Topic for " + tarChannel->name + ": " + tarChannel->getChannelTopic() + "\r\n" + "Topic set by " + tarChannel->topicSetter + "[server hostname]" + " " + tarChannel->topicSetTime + "\r\n";
		send(clientSocket, topicMsg.c_str(), topicMsg.size(), 0);
	}
}

