#include "Server.hpp"
#include "Msg.hpp"

int	Server::topicCommand(Msg msg, int clientSocket, Client &client)
{
	/* check msg.parameter size,
		if (size == 1) -> get topic
		else if (size == 2) -> set topic
			check if topic_requiredOp is true 
		else if (size == 3) -> find the channel and set topic 
			check if topic_requiredOp is true 
		else -> error message	
	*/
	if (msg.parameters.size() == 1 && msg.trailing_msg.empty())
	{
		topicPrint(msg, clientSocket, client);
	}
	else if (!msg.trailing_msg.empty())
	{
		for (std::vector<Channel>::iterator it = channel_names.begin(); it != channel_names.end(); it++)
		{
			if (it->name == msg.parameters[0])
			{
				std::cout << "debug 1" << std::endl;
				if (it->topic_requires_operator)
				{
					std::cout << "debug 2" << std::endl;
					for (auto &setter : it->channel_users)
					{
						if (setter.nickname == client.getNickname())
						{
							if (!setter.operator_permissions)
							{
								std::cout << "debug 3" << std::endl;
								std::string error = ":ircserv NOTICE " + client.getNickname() + " :You do not have the required operator status to change the topic\r\n";
								send(clientSocket, error.c_str(), error.size(), 0);
								return 1;
							}
							else
							{
								std::cout << "debug 4" << std::endl;
								it->setChannelTopic(msg.trailing_msg, client);
								std::string topicMsg = ":ircserv TOPIC " + it->name + " :" + msg.trailing_msg + "\r\n";
								broadcastToChannel(*it, topicMsg);
							}
						}
					}
				}
				else
				{
					std::cout << "debug 5" << std::endl;
					it->setChannelTopic(msg.trailing_msg, client);
					std::string topicMsg = ":ircserv TOPIC " + it->name + " :" + msg.trailing_msg + "\r\n";
					broadcastToChannel(*it, topicMsg);
					return 1;
				}
			}
		}
	}
	else
	{
		std::string error = ":ircserv NOTICE " + client.getNickname() + " :Invalid number of parameters for TOPIC command\r\n";
		send(clientSocket, error.c_str(), error.size(), 0);
	}
	return 0;
}

void Server::topicPrint(Msg msg, int clientSocket, Client &client)
{
	for (std::vector<Channel>::iterator it = channel_names.begin(); it != channel_names.end(); it++)
	{
		if (it->name == msg.parameters[0])
		{
			std::string channelTopic = it->getChannelTopic();
			std::string topicMsg = "Topic for " + msg.parameters[0] + ": " + channelTopic + "\r\n" + "Topic set by " + it->topicSetter + "[server hostname]" + " " + it->topicSetTime + "\r\n";
			send(clientSocket, topicMsg.c_str(), topicMsg.size(), 0);
		}
	}
}
