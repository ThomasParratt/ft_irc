#include "Server.hpp"
#include "Msg.hpp"


/*
9/1 currently, it is not entering this function when we just type /topic
*/
int	Server::topicCommand(Msg msg, int clientSocket, Client &client)
{
	printMsg(msg); // debug
	if (msg.trailing_msg.empty()) 
	{
		std::cout << "debug: just /topic" << std::endl;
		topicPrint(msg.parameters[0], clientSocket, client);
		return (0);
	}
	else
	{
		int i = getChannelIndex(msg.parameters[0], channel_names);
		if (i == -1)
		{
			std::string notice = ":ircserv 403 " + client.getNickname() + " " + msg.parameters[0] + " :No such channel\r\n";
			send(clientSocket, notice.c_str(), notice.size(), 0);
			return (1);
		}
		if (channel_names[i].getTopicRequiresOperator() && clientStatus(msg, client) == 0)
		{
			std::string errMsg = ":ircserver 482 " + client.getNickname() + " " + msg.parameters[0] + " :You're not a channel operator\r\n";
			send(clientSocket, errMsg.c_str(), errMsg.size(), 0);
			return (1);
		}
		channel_names[i].setChannelTopic(msg.trailing_msg, client);
		std::string topicMsg = ":" + client.getNickname() + " TOPIC " + channel_names[i].getChannelName() + " :" + msg.trailing_msg + "\r\n";
		broadcastToChannel(channel_names[i], topicMsg);
	}
	return 0;
}

void Server::topicPrint(std::string channelName, int clientSocket, Client &client)
{
	for (std::vector<Channel>::iterator it = channel_names.begin(); it != channel_names.end(); it++)
	{
		if (it->name == channelName)
		{
            std::string channelTopic = it->getChannelTopic();
			if (channelTopic.empty())
			{
				std::string noTopic = ":ircserver 331 " + client.getNickname() + " " + channelName + " :No topic is set\r\n";
            	send(clientSocket, noTopic.c_str(), noTopic.size(), 0);
				break;
			}
            std::string topicMsg = ":ircserver 332 " + client.getNickname() + " " + channelName + " :" + channelTopic + "\r\n";
            send(clientSocket, topicMsg.c_str(), topicMsg.size(), 0);
            std::string topicSetByMsg = ":ircserver 333 " + client.getNickname() + " " + channelName + " " + it->topicSetter + " " + it->topicSetTime + "\r\n";
            send(clientSocket, topicSetByMsg.c_str(), topicSetByMsg.size(), 0);
            break;
		}
	}
}
