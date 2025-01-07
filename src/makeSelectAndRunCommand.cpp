//libraries
#include <iostream>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>//send
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

//header files
#include "Msg.hpp"
#include "Channel.hpp"
#include "Server.hpp"

int		Server::commandSelector(Msg msg, int clientSocket, Client &client)
{
	// std::cout << "Command Selector: " << msg.command <<  std::endl;
	
	if (msg.command == "CAP")
	{
		
	}
	else if (msg.command == "PASS")
	{
		if (Server::passwordCommand(msg, clientSocket, client) != 0)// To Do: If password incorrect...
		{
			return (1);
		}
	}
	else if (msg.command == "NICK")
	{
		Server::nicknameCommand(msg, clientSocket, client);
	}
	else if (msg.command == "USER")
	{
		if (Server::userCommand(msg, clientSocket, client) != 0)
			return (1);
	}
	// else if (msg.command == "OPER")
	// {
	// 	Server::operCommand(msg, clientSocket, client);
	// }
	else if  (msg.command == "PING")
	{
		std::string response = "PONG " + msg.parameters[0] + "\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
	}
	else if  (msg.command == "PRIVMSG")
	{
		privmsgCommand(msg, clientSocket, client);
	}
	else if  (msg.command == "KICK")
	{
		kickCommand(msg, clientSocket, client);
	}
	else if  (msg.command == "INVITE")
	{
		inviteCommand(msg, clientSocket, client);
	}
	else if (msg.command == "PART")
	{
		partCommand(msg, clientSocket, client);
	}
	else if  (msg.command == "TOPIC")
	{
		topicCommand(msg, clientSocket, client);
	}
	else if  (msg.command == "MODE")
	{
		//modeCommand(msg, clientSocket, client);
			//TO DO: Change channel's mode
			//parse the command
			// /MODE <channel> +i = invite only | /MODE <channel> -i = remove invite only
			// /MODE <channel> +t = only operator can change topic | /MODE <channel> -t = anyone can change the topic
			// /MODE <channel> +k <password> = add the password to the channel | /MODE <channel> -k = remove the password from the channel	
			// /MODE <channel> +o <nickname> = give operator status | /MODE <channel> -o <nickname> = remove operator status
			// /MODE <channel> +l <number> = set the limit of users in the channel | /MODE <channel> -l = remove the limit of users in the channel
		modeCommand(msg, clientSocket, client);
	}	
	else if (msg.command == "JOIN")
	{
		joinCommand(msg, clientSocket, client);
	}
	else if (msg.command[0] == ':')
	{
		std::string response = "Please no Prefixes in Commands. Server No like.\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
	}
	else
	{
		std::string response = "ft_irc: Command not found: " + msg.command + "\r\n";
		send(clientSocket, response.c_str(), response.size(), 0);
	}
	return (0);
}


int    Server::makeSelectAndRunCommand(std::string messages, int clientSocket, Client &client)
{
	std::vector<Msg>     msgs;
    
    this -> makeMessages(msgs, messages);

	for (int i = 0; i < msgs.size(); i++)
	{
		if (this->commandSelector(msgs[i], clientSocket, client) == 1)
		{
			//TO DO. If error etc...
			return (1);
		}
	}
	return (0);
}