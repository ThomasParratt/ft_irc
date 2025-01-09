
#include "Server.hpp"


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
	if (clientStatus(msg, client) == 0)
	{
		std::string errMsg = ":ircserver 482 " + client.getNickname() + " " + msg.parameters[0] + " :You're not a channel operator\r\n";
		send(clientSocket, errMsg.c_str(), errMsg.size(), 0);
		return (1);
	}
	Channel* tarChannel = getChannel(msg.parameters[0]);
	if (tarChannel == nullptr)
	{
		std::string message = ":" + msg.parameters[0] + " :No such channel\r\n";
		send(clientSocket, message.c_str(), message.size(), 0);
		return (1);
	}
	if (msg.parameters[1].empty())
	{
		std::string u_modeIs = ":ircserver 221 " + client.getNickname() + " User Modes=i,t,k,o,l" + "\r\n";
    	send(client.getSocket(), u_modeIs.c_str(), u_modeIs.size(), 0);
		return(1);
	}
	else if (msg.parameters[1] == "+i" || msg.parameters[1] == "-i")
	{
		tarChannel->invite_only = (msg.parameters[1] == "+i") ? true : false;
		std::string inviteMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " " + msg.parameters[1] + "\r\n";
		broadcastToChannel(*tarChannel, inviteMsg);
	}
	else if (msg.parameters[1] == "+t" || msg.parameters[1] == "-t")
	{
		tarChannel->topic_requires_operator = (msg.parameters[1] == "+t") ? true : false;
		std::string topicMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " " + msg.parameters[1] + "\r\n";
		broadcastToChannel(*tarChannel, topicMsg);
	}
	else if (msg.parameters[1] == "+k" || msg.parameters[1] == "-k")
	{
		if (msg.parameters[1] == "+k" && msg.parameters[2].empty()) //check if key is empty or not. not sure if this is the right error code to return
		{
			std::string errMsg = ":ircserver 525" + client.getNickname() + " " + msg.parameters[0] + " :Key is not well-formed\r\n";
			send(clientSocket, errMsg.c_str(), errMsg.size(), 0);
			return (1);
		}
		else if (msg.parameters[1] == "+k") {
			// std::cout << "Setting Channel Key: " << msg.parameters[2] << std::endl; //debug
			tarChannel->channel_key = msg.parameters[2];
			tarChannel->keyRequired = true;
			std::string keyMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " +k " + msg.parameters[2] + "\r\n";
			broadcastToChannel(*tarChannel, keyMsg);
		}
		else if (msg.parameters[1] == "-k")
		{
			// std::cout << "Removing Channel Key" << std::endl; //debug
			tarChannel->channel_key = "";
			tarChannel->keyRequired = false;
			std::string keyMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " -k *" + "\r\n";
			broadcastToChannel(*tarChannel, keyMsg);
		}
	}
	else if (msg.parameters[1] == "+o" || msg.parameters[1] == "-o")
	{			
		if (msg.parameters[2].empty())
		{
			std::string errMsg = ":ircserver 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
			send(clientSocket, errMsg.c_str(), errMsg.size(), 0);
			return (1);
		}
		bool nickExists = false;
		for (int i = 0; i < tarChannel->channel_users.size(); i++)
		{
			if (tarChannel->channel_users[i].nickname == msg.parameters[2])
			{
				nickExists = true;
				tarChannel->channel_users[i].operator_permissions = (msg.parameters[1] == "+o") ? true : false;
				std::string chMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " " + msg.parameters[1] + " " + msg.parameters[2] + "\r\n";
				broadcastToChannel(*tarChannel, chMsg);
				break;
			}
		}
		if (nickExists == false)
		{
			std::string errMsg = ":ircserver 441 " + client.getNickname() + " " + msg.parameters[2] + " " + msg.parameters[0] + " :They aren't on that channel\r\n";
			send(clientSocket, errMsg.c_str(), errMsg.size(), 0);
			return (1);
		}
	}
	else if (msg.parameters[1] == "+l" || msg.parameters[1] == "-l")
	{
		std::cout << "l" << std::endl; //debug
		tarChannel->user_limit = (msg.parameters[1] == "+l") ? std::stoi(msg.parameters[2]) : 0;
		std::string limitMsg;
		if (msg.parameters[1] == "+l")
		{
			if (msg.parameters[2].empty())
			{
				std::string errMsg = ":ircserver 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
				send(clientSocket, errMsg.c_str(), errMsg.size(), 0);
				return (1);
			}
			else 
				limitMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " " + msg.parameters[1] + " " + msg.parameters[2] + "\r\n";
		}
		else
		{
			limitMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " " + msg.parameters[1] + "\r\n";
		}
		broadcastToChannel(*tarChannel, limitMsg);
	}
	else
	{
		std::string errMsg = ":ircserver 472" + client.getNickname() + " " + msg.parameters[1] + " :is unknown mode char to me\r\n";
		send(clientSocket, errMsg.c_str(), errMsg.size(), 0);
		return (1);
	}
	return (0);
}
