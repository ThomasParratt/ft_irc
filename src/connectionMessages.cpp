#include "Server.hpp"
#include "Msg.hpp"


int		Server::passwordCommand(Msg msg, int clientSocket, Client &client)
{
	if (msg.parameters[0] != client.getPassword())
	{
		std::string message_464 = ":ircserv 464 * :Password incorrect\r\n";
		send(clientSocket, message_464.c_str(), message_464.size(), 0);
		return (1);
	}
	client.setPasswordChecked(true);
	return (0);
}

int Server::nickClash(const std::string& nickname, int socket)
{
    // Check if any other client has the same nickname
    for (auto &client : clients) 
    {
		if (client.getSocket() != socket)
		{
        	if (client.getNickname() == nickname)
            	return (1); // Nickname is taken
		}
    }
    return (0); // Nickname is available
}

int		Server::nicknameCommand(Msg msg, int clientSocket, Client &client)
{
	if (!client.getWelcomeSent() && client.getNickname().empty())
	{
		client.setNickname(msg.parameters[0]);
	}
	else if (msg.parameters[0].empty()) //ERR_NONICKNAMEGIVEN (431)
	{
		std::string message_431 = ":ircserv 431 " + client.getNickname() + " :No nickname given\r\n";
		send(clientSocket, message_431.c_str(), message_431.size(), 0);
	}
	// else if ()
	// {
	// 	//ERR_ERRONEUSNICKNAME (432)
	// }
	else if (this->nickClash(msg.parameters[0], clientSocket)) //ERR_NICKNAMEINUSE (433)
	{
		std::string message_433 = ":ircserv 433 " + msg.parameters[0] + " :" + msg.parameters[0] + "\r\n";
		send(clientSocket, message_433.c_str(), message_433.size(), 0);
	}
	else
	{
		//Change nickname 
		std::string nick_message1 = ":" + client.getNickname() + " NICK ";
		client.setNickname(msg.parameters[0]);
		std::string nick_message2 = client.getNickname() + "\r\n";
		std::string nick_message = nick_message1 + nick_message2;
		send(clientSocket, nick_message.c_str(), nick_message.size(), 0);
	}
	return (0);
}

int		Server::userCommand(Msg msg, int clientSocket, Client &client)
{
	if (client.getPasswordChecked())
	{
		client.setUsername(msg.parameters[0]);
		std::string message_001 = ":ircserv 001 " + client.getNickname() + " :Welcome to the IRC network " + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostIP() + "\r\n";
		send(clientSocket, message_001.c_str(), message_001.size(), 0);
		std::string message_002 = ":ircserv 002 " + client.getNickname() + " :Your host is ircserv, running version 1.0\r\n";
		send(clientSocket, message_002.c_str(), message_002.size(), 0);
		std::string message_003 = ":ircserv 003 " + client.getNickname() + " :This server was created " + this->getStartTimeStr() + "\r\n";
		send(clientSocket, message_003.c_str(), message_003.size(), 0);
		std::string message_004 = ":ircserv 004 " + client.getNickname() + " ircserv 1.0 ro itkol\r\n";
		send(clientSocket, message_004.c_str(), message_004.size(), 0);
		std::string message_005 = ":ircserv 005 " + client.getNickname() + " CHANMODES=i,t,k,o,l :are supported by this server\r\n";
		send(clientSocket, message_005.c_str(), message_005.size(), 0);
		if (this->nickClash(client.getNickname(), clientSocket))
		{
			std::string nick_message1 = ":" + client.getNickname() + " NICK ";
			client.setNickname(client.getNickname() + "_" + std::to_string(clientSocket));
			std::string nick_message2 = client.getNickname() + "\r\n";
			std::string nick_message = nick_message1 + nick_message2;
			send(clientSocket, nick_message.c_str(), nick_message.size(), 0);
		}
		client.setWelcomeSent(true);
		return (0);
	}
	else
	{
		std::string message_464 = ":ircserv 464 * :Password needed\r\n";
		send(clientSocket, message_464.c_str(), message_464.size(), 0);
		return (1);
	}
}
