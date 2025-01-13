
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

int		Server::userLimitMode(Msg msg, Client &client, Channel *tarChannel)
{
		std::string 	user_limit_string;
		std::string 	limitMsg;

		// std::cout << "userLimitMode" << std::endl; //debug

		if (msg.parameters[1] == "-l")
		{
			tarChannel -> user_limit = -1;
			limitMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " " + msg.parameters[1] + "\r\n";
		}
		else
		{
			if (msg.parameters[1] == "+l")
			{
				// No Limit given
				std::cout << "No Limit Given" << std::endl;//debug
				return (1);
			}
			else
			{
				// std::cout << "Limit Given" << std::endl;
				std::string user_limit_string;
				user_limit_string = msg.parameters[1].substr(2);	//Note: substring gets rid of "+l" and allows us to get limit number

				int userLimit;	
				try
				{
					userLimit = std::stoi(user_limit_string);
				}
				catch(const std::exception& e)
				{
					//Not an integer
					// std::cerr << "Error: " << e.what() << '\n';
					return (1);
				}
				// std::cout << "userLimit: " << userLimit << std::endl;
				if (userLimit <= 0)
				{
					//If Limit is zero or below
					// std::cout << "Limit is an unacceptable value: " << userLimit << std::endl;
					return (1);
				}
				// std::cout << "userLimit (int): " << userLimit << std::endl;

				tarChannel -> user_limit = userLimit;

				msg.parameters[1] = "+l";

				limitMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " " + msg.parameters[1] + " " + user_limit_string + "\r\n";
				// std::cout << "limitMsg (+l): " << limitMsg;	
			}
		}

		broadcastToChannel(*tarChannel, limitMsg);
		return (0);
}

int		Server::channelChecks(Msg msg, Client &client)
{
	if (msg.parameters.size() == 0)
	{
		//Not enough Parameters
		return (1);
	}
	if (msg.parameters[0][0] != '#')
	{
		/*
			Check if not a Channel
				-> ignore users (f.ex. MODE userA +i) 
		*/
		return (1);		
	}
	if (msg.parameters.size() == 1)
	{
		//Check special command: MODE #Channel

		// :ircserver 324 mkorpela #ABC +n
		// std::string msg = ":ircserver 324 mkorpela #ABC +n\r\n";

		std::string messages = ":ircserv 324 " + client.getNickname() +  " " + msg.parameters[0] + " +n\r\n";//User can send message to channel, only if they are in the channel.
		send(client.getSocket(), messages.c_str(), messages.size(), 0);
		return (1);
	}
	if (channelExists(msg.parameters[0]) == 0)
	{
		std::string errMsg = ":ircserv 482 " + client.getNickname() + " " + msg.parameters[0] + " :Channel doesn't exist\r\n";
		send(client.getSocket(), errMsg.c_str(), errMsg.size(), 0);
		return (1);
	}
	if (clientStatus(msg, client) == 0)	//Check if not a channel operator
	{
		std::string errMsg = ":ircserv 482 " + client.getNickname() + " " + msg.parameters[0] + " :You're not a channel operator\r\n";
		send(client.getSocket(), errMsg.c_str(), errMsg.size(), 0);
		return (1);
	}
	return (0);
}

int		Server::keyMode(Msg msg, Client &client, Channel* tarChannel)
{
	std::cout << msg.parameters[1] << std::endl; //debug
	if (msg.parameters[1] == "+k")
	{
		if (msg.parameters.size() == 2)
		{
			//No key given
			return (1);
		}
		else if(msg.parameters[2] == "")//Empty string for a key - not allowed
		{
			std::cout << "+k/-k" << std::endl;
			std::cout << "Key is empty" << std::endl; //debug
			std::string errMsg = ":ircserver 525" + client.getNickname() + " " + msg.parameters[0] + " :Key is not well-formed\r\n";
			send(client.getSocket(), errMsg.c_str(), errMsg.size(), 0);
			return (1);
		}
		else if (msg.parameters[2] != "")
		{
			std::cout << "Setting Channel Key: " << msg.parameters[2] << std::endl; //debug
			tarChannel->channel_key = msg.parameters[2];
			tarChannel->keyRequired = true;
			std::string keyMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " +k " + msg.parameters[2] + "\r\n";
			broadcastToChannel(*tarChannel, keyMsg);
			return (0);
		}
	}
	else if (msg.parameters[1] == "-k")
	{
		std::cout << "Removing Channel Key" << std::endl; //debug
		tarChannel->channel_key = "";
		tarChannel->keyRequired = false;
		std::string keyMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " -k *" + "\r\n";
		broadcastToChannel(*tarChannel, keyMsg);
		return (0);
	}
	return (0);
}

void	Server::topicMode(Msg msg, Client &client, Channel* tarChannel)
{
	tarChannel->topic_requires_operator = (msg.parameters[1] == "+t") ? true : false;
	std::cout << "+t/-t" << std::endl; //debug
	std::string topicMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " " + msg.parameters[1] + "\r\n";
	broadcastToChannel(*tarChannel, topicMsg);
}

void	Server::inviteMode(Msg msg, Client &client, Channel* tarChannel)
{
	std::cout << "+i/-i" << std::endl; //debug
	tarChannel->invite_only = (msg.parameters[1] == "+i") ? true : false;
	std::string inviteMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " " + msg.parameters[1] + "\r\n";
	broadcastToChannel(*tarChannel, inviteMsg);
}

int		Server::operatorMode(Msg msg, Client &client, Channel* tarChannel)
{
	std::cout << "o" << std::endl; //debug
	//TODO: Give/take channel operator privilege
	// need to check if names is given before
	if (msg.parameters[2].empty())
	{
		std::string errMsg = ":ircserver 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
		send(client.getSocket(), errMsg.c_str(), errMsg.size(), 0);
		return (1);
	}
	bool nickExists = false;
	for (int i = 0; i < tarChannel->channel_users.size(); i++)
	{
		if (tarChannel->channel_users[i].nickname == msg.parameters[2])
		{
			// need to make sure it is not the client itself
			nickExists = true;
			std::cout << "found: " << tarChannel->channel_users[i].nickname << std::endl; //debug
			std::cout << tarChannel->channel_users[i].operator_permissions << std::endl; //debug
			tarChannel->channel_users[i].operator_permissions = (msg.parameters[1] == "+o") ? true : false;
			std::cout << tarChannel->channel_users[i].operator_permissions << std::endl; //debug
			std::string chMsg = ":" + client.getNickname() + " MODE " + msg.parameters[0] + " " + msg.parameters[1] + " " + msg.parameters[2] + "\r\n";
			broadcastToChannel(*tarChannel, chMsg);
			break;
		}
	}
	if (nickExists == false)
	{
		std::string errMsg = ":ircserver 441 " + client.getNickname() + " " + msg.parameters[2] + " " + msg.parameters[0] + " :They aren't on that channel\r\n";
		send(client.getSocket(), errMsg.c_str(), errMsg.size(), 0);
		return (1);
	}
	return (0);
}

int		Server::modeCommand(Msg msg, int clientSocket, Client &client)
{
	//Figure out edge case - MODE mkorpela +i -> the below error message will print - edge case must be ignored -> check for #...
	std::cout << "MODE command" << std::endl;

	if (channelChecks(msg, client) != 0)
	{
		return (1);
	}

	printMsg(msg); //debug
	std::cout << "1" << std::endl;

	//find the current channel to apply the mode + flag
	Channel* tarChannel = getChannel(msg.parameters[0]);
	if (tarChannel == nullptr)
	{
		std::string message = ":" + msg.parameters[0] + " :No such channel\r\n";
		send(clientSocket, message.c_str(), message.size(), 0);
		return (1);
	}
	std::cout << "2" << std::endl;	

	if (msg.parameters[1] == "+i" || msg.parameters[1] == "-i")
	{
		inviteMode(msg, client, tarChannel);
	}
	else if (msg.parameters[1] == "+t" || msg.parameters[1] == "-t")
	{
		topicMode(msg, client, tarChannel);
	}
	else if (msg.parameters[1] == "+k" || msg.parameters[1] == "-k")
	{
		keyMode(msg, client, tarChannel);
	}
	else if (msg.parameters[1] == "+o" || msg.parameters[1] == "-o")
	{			
		operatorMode(msg, client, tarChannel);
	}
	else if (msg.parameters[1] == "+l" || msg.parameters[1] == "-l" || (msg.parameters[1][0] == '+' && msg.parameters[1][1] == 'l'))
	{
		userLimitMode(msg, client, tarChannel);//Note to self - check for error?
	}
	else
	{
		/*
			Unknown Mode Command
				-> ignore
		*/
		std::cout << "MODE: unknown command" << std::endl; //debug
		return (1);
	}
	return (0);
}
