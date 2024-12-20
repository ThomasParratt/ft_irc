
#include "Server.hpp"

int		Server::checkErrorInMode(Msg msg, int clientSocket)
{
	if (msg.parameters.size() <= 1)
	{
		return (1);
	}
	else if (msg.parameters[0][0] != '#')
	{
		return (1);
	}
	else if (msg.parameters[1].empty())
	{
		return (1);
	}
	return (0);
}

int 	Server::clientStatus(Msg msg, Client &client)
{
	for (auto &channel : channel_names)
	{
		if (channel.name == msg.parameters[0])
		{
			for (auto &users : channel.channel_users)
			{
				if (users.nickname == client.getNickname())
				{
					if (users.operator_permissions)
						return (1); // Operator status
				}
			}
		}
	}
	return (0); // no Operator status
}

Channel* Server::getChannel(std::string channelName)
{
	for (auto &channel : channel_names)
	{
		if (channel.name == channelName)
			return &channel;
	}
	return nullptr;
}

int		Server::modeCommand(Msg msg, int clientSocket, Client &client)
{
	if (checkErrorInMode(msg, clientSocket) == 1)
		return (1);
	if (clientStatus(msg, client) == 0)
	{
		std::string errMsg = ":" + client.getNickname() + " MODE " + msg.parameters[1] + " :You're not a channel operator\r\n";
		send(clientSocket, errMsg.c_str(), errMsg.size(), 0);
		return (1);
	}
	//find the current channel to apply the mode + flag
	Channel* tarChannel = getChannel(msg.parameters[0]);
	if (tarChannel == nullptr)
	{
		std::string message = ":" + msg.parameters[0] + " :No such channel\r\n";
		send(clientSocket, message.c_str(), message.size(), 0);
		return (1);
	}
	//parse the command
	std::cout << "Channel name: " << tarChannel->name << std::endl; //debug
	if (msg.parameters[1] == "+i" || msg.parameters[1] == "-i")
	{
		std::cout << "i" << std::endl; //debug
		tarChannel->invite_only = (msg.parameters[1] == "+i") ? true : false;
	}
	else if (msg.parameters[1] == "+t" || msg.parameters[1] == "-t")
	{
		tarChannel->topic_requires_operator = (msg.parameters[1] == "+t") ? true : false;
	}
	else if (msg.parameters[1] == "+k" || msg.parameters[1] == "-k")
	{
		std::cout << "k" << std::endl; //debug
		std::cout << "Setting Channel Key: " << msg.parameters[2] << std::endl; //debug
		tarChannel->channel_key = (msg.parameters[1] == "+k") ? msg.parameters[2] : "";
	}
	else if (msg.parameters[1] == "+o" || msg.parameters[1] == "-o")
	{			
		std::cout << "o" << std::endl; //debug
		//TODO: Give/take channel operator privilege
		for (int i = 0; i < tarChannel->channel_users.size(); i++)
		{
			if (tarChannel->channel_users[i].nickname == msg.parameters[2])
			{
				std::cout << "found: " << tarChannel->channel_users[i].nickname << std::endl; //debug
				std::cout << tarChannel->channel_users[i].operator_permissions << std::endl; //debug
				tarChannel->channel_users[i].operator_permissions = (msg.parameters[1] == "+o") ? true : false;
				std::cout << tarChannel->channel_users[i].operator_permissions << std::endl; //debug
				break;
			}
		}
	}
	else if (msg.parameters[1] == "+l" || msg.parameters[1] == "-l")
	{
		std::cout << "l" << std::endl; //debug
		tarChannel->user_limit = (msg.parameters[1] == "+l") ? std::stoi(msg.parameters[2]) : 0;
	}
	else
	{
		std::string errMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " :Unknown MODE flag\r\n";
		send(clientSocket, errMsg.c_str(), errMsg.size(), 0);
	}
	return (0);
}
