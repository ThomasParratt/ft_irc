#include <iostream>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>//send
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>





#include "Msg.hpp"
#include "Channel.hpp"
#include "Server.hpp"





size_t	skipSpaces(std::string string, size_t index)
{
	while (string[index] == ' ')
	{
		index++;
	}
	return (index);
}

int     count_msgs(std::string buffer)
{
    int count = 0;

    size_t		start = 0;
	size_t		end = 0;

	while ((end = buffer.find("\r\n", start)) != std::string::npos)
	{
		start = end + 2;
        count++;								
    }    
    return (count);
}

void replaceChar(std::string &buffer, char old_char, char new_char)         //Util-like function
{
    for (char &c:buffer)
    {
        if (c == old_char)
            c = new_char;
    }
}


std::string		getCommand(std::vector<std::string> array)
{
	return (array[0]);
}

std::vector<std::string>	getParameters(std::vector<std::string> array)
{
	std::vector<std::string> parameters;

	for (int i = 1; i < array.size(); i++)
	{
		if (array[i][0] != ':')
		{
			// std::cout << "Params: " << array[i] << std::endl;
			parameters.push_back(array[i]);
		}
    }
	return (parameters);
}

std::string		getTrailingMessage(std::vector<std::string> array)
{
	size_t last_string = array.size() - 1;

	if (array[last_string][0] == ':')
	{
		// std::cout << "Trailing Message: " << &(array[last_string][1]) << std::endl;		
		return (&(array[last_string][1]));			//getting rid of 
	}
	else
	{
		return ("");
	}
}

int		Server::joinCommand(Msg msg, int clientSocket, Client &client)
{
	//Welcome Msg for channel?
	/*
		1. Check if channel exists
		a. If No
			i. Create Channel (= create Channel Object)
			ii. Add User to
				-channel_user - Nickname
				-permission  - operator
			iii. Send Welcome message

		b. If Yes
			- join channel - consider password
	*/
	/*
		Code:
			Search for channel
				-> If Find
					-> Return channel?
	*/

	//If Channel Doesn't exist
	Channel new_channel(msg.parameters[0]);

	//Make new Channel User
	User	new_user;
	new_user.nickname = client.getNickname();
	new_user.operator_permissions = true;

	//Add into channel Users
	new_channel.channel_users.push_back(new_user);

	//Send Welcome msg to User
  	std::string response = "Welcome to the Channel " + msg.parameters[0] + "\r\n";
	// send(clientSocket, "Channel Welcome Message\r\n", 26, 0);
    send(clientSocket, response.c_str(), response.size(), 0);// Note this isn't working.

	this->channel_names.push_back(new_channel);

	/*
		Send This!
			:sender_nickname!user@host PRIVMSG #channel_name :message_text
			:Alice!alice@irc.example.com PRIVMSG #general :Hello everyone!
	*/

	//Push channel into channel vector on Server

	//push back into server [at the end]

	return (0);
}

void    initializeMsg(Msg &msg, std::vector<std::string> array)
{	
	msg.command = getCommand(array);
	msg.parameters = getParameters(array);
	msg.trailing_msg = getTrailingMessage(array);
}

int	count_words(std::string buffer)
{
	int	count = 0;

	int i = 0;
	while (buffer[i])
	{
		if (buffer[i] == ' ')
		{
			i++;
		}
		else
		{
			count++;
			while(buffer[i] != ' ' && buffer[i] != '\0')
			{
				i++;
			}
		}
	}
	return (count);
}

/*
	Splits a String separated by Spaces to an Array.
	std::string ---> std::vector<std::string>
*/
void	stringToArray(std::string string, std::vector<std::string> &array)
{
    std::string	word;
	size_t		start = 0;
	size_t		end = 0;
	size_t		length = 0;

	start = skipSpaces(string, start);
	while (true)
	{
		//Find index of next space
		end = string.find(' ', start);
		if (end == std::string::npos || string[start] == ':')	//Extract last word, if a. space character IS NOT found or b. if trailing message IS found
		{
			word = string.substr(start);
			array.push_back(word);
			break ;
		}
		else							//otherwise calculate length of word and extract
		{
			length = end - start;

			word = string.substr(start, length);
			array.push_back(word);

			end = skipSpaces(string, end);
			start = end;
		}
	}
}
/*
	Replaces traling carriage return ('\r') and newline ('\n) to null terminator.
*/
void	removeStringEnd(std::string &message)
{
    replaceChar(message, '\n', '\0');
    replaceChar(message, '\r', '\0');
}
/*
	Checks for Prefix in message. Irssi shouldn't be sending a prefix.
*/
bool	CheckIfPrefix(std::string buffer)
{
	if (buffer[0] == ':')
	{
		std::cout << "Error: Please don't send a Prefix through Irssi. We don't handle those." << std::endl;
		return (true);
	}
	return (false);
}

void	printMsg(Msg msg)
{
	std::cout << "Command: " << msg.command << std::endl;
	for (int i = 0;  i < msg.parameters.size();  i++)
	{
		std::cout  << "Parameter[" << i << "]: " << msg.parameters[i] << std::endl;
	}
	if (msg.trailing_msg != "")
	{
		std::cout << "Trailing_msg: " << msg.trailing_msg << std::endl;
	}
}

void	printArray(std::vector<std::string>message_array)
{
    for (size_t i = 0; i < message_array.size(); i++)
	{
        std::cout << "Array: " << message_array[i] << std::endl;
    }
}

void	makeMsgfromString(Msg &msg, std::string message)
{
    std::vector<std::string>	message_array;

	// std::cout << "message: " << message << std::endl;

	//Array gets rid of spaces between Words (except for spaces within trailing msg)
	stringToArray(message, message_array);
	// printArray(message_array);

    initializeMsg(msg, message_array);
	// printMsg(msg);

	return ;
}

void	Server::makeMessages(std::vector<Msg> &msgs, std::string buffer)
{
	Msg				msg;

	std::string		single_msg;
    size_t			start = 0;
	size_t			end = 0;
	size_t			length = 0;

	while ((end = buffer.find("\r", start)) != std::string::npos)
	{
		length = end - start;
		single_msg = buffer.substr(start, length);
		// std::cout << "single_msg: " << single_msg << std::endl;

		makeMsgfromString(msg, single_msg);
		msgs.push_back(msg);
		start = end + 2;
	}
}

int		Server::passwordCommand(Msg msg, int clientSocket, Client &client)
{
	if (msg.parameters[0] != client.getPassword())
	{
		std::string message_464 = ":ircserv 464 * :Password incorrect\r\n";
		send(clientSocket, message_464.c_str(), message_464.size(), 0);

		return (1);
	}
	return (0);
}

int Server::clientLoop(const std::string& nickname)
{
    // Check if any other client has the same nickname
    for (auto &client : clients) 
    {
        if (client.getNickname() == nickname)
            return (1); // Nickname is taken
    }
    return (0); // Nickname is available
}

int		Server::nicknameCommand(Msg msg, int clientSocket, Client &client)
{
	if (!client.getWelcomeSent() && client.getNickname().empty())
	{
		client.setNickname(msg.parameters[0]);
		std::string message_001 = ":ircserv 001 " + client.getNickname() + " :Welcome to the IRC network " + client.getNickname() + "!" + client.getNickname() + "@" + this->getServHostName() + "\r\n";
		send(clientSocket, message_001.c_str(), message_001.size(), 0);
		std::string message_002 = ":ircserv 002 " + client.getNickname() + " :Your host is ircserv, running version 1.0\r\n";
		send(clientSocket, message_002.c_str(), message_002.size(), 0);
		std::string message_003 = ":ircserv 003 " + client.getNickname() + " :This server was created at " + this->getStartTimeStr() + "\r\n";
		send(clientSocket, message_003.c_str(), message_003.size(), 0);
		std::string message_004 = ":ircserv 004 " + client.getNickname() + " ircserv 1.0 ro itkol\r\n";
		send(clientSocket, message_004.c_str(), message_004.size(), 0);
		std::string message_005 = ":ircserv 005 " + client.getNickname() + " CHANMODES=i,t,k,o,l :are supported by this server\r\n";
		send(clientSocket, message_005.c_str(), message_005.size(), 0);
		client.setWelcomeSent(true);
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
	else if (this->clientLoop(msg.parameters[0])) //ERR_NICKNAMEINUSE (433)
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

// int		Server::operCommand(Msg msg, int clientSocket, Client &client)
// {
// 	client.setOperatorStatus(true);
// 	std::string message_381 = ":ircserv 381 " + client.getNickname() + " :You are now an IRC operator\r\n";
// 	send(clientSocket, message_381.c_str(), message_381.size(), 0);
// 	return (0);
// }

int		Server::commandSelector(Msg msg, int clientSocket, Client &client)
{
	// std::cout << "Command Selector: " << msg.command <<  std::endl;
	
	if (msg.command == "CAP")
	{
		//Do nothing?
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
		client.setUsername(msg.parameters[0]);
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

	}
	else if  (msg.command == "KICK")
	{
		if (client.getOperatorStatus())
		{
			//TO DO: Kick User
		}
		else
		{
			//TO DO: Send Error Message "User does not have Operator Status"
		}
	}
	else if  (msg.command == "INVITE")
	{
		if (client.getOperatorStatus())
		{
			//TO DO: Invite User
		}
		else
		{
			//TO DO: Send Error Message "User does not have Operator Status"
		}
	}
	else if  (msg.command == "TOPIC")
	{
		if (client.getOperatorStatus())
		{
			//TO DO: change channel topic
		}
		else
		{
			//TO DO: Send Error Message "User does not have Operator Status"
		}
	}
	else if  (msg.command == "MODE")
	{
		if (client.getOperatorStatus())
		{
			//TO DO: Change channel's mode
			//parse the command
			// /MODE <channel> +i = invite only | /MODE <channel> -i = remove invite only
			// /MODE <channel> +t = only operator can change topic | /MODE <channel> -t = anyone can change the topic
			// /MODE <channel> +k <password> = add the password to the channel | /MODE <channel> -k = remove the password from the channel	
			// /MODE <channel> +o <nickname> = give operator status | /MODE <channel> -o <nickname> = remove operator status
			// /MODE <channel> +l <number> = set the limit of users in the channel | /MODE <channel> -l = remove the limit of users in the channel
		}	
		else
		{
			//TO DO: Send Error Message "User does not have Operator Status"
		}
	}	
	else if (msg.command == "JOIN")
	{
		joinCommand(msg, clientSocket, client);
	}
	else if (msg.command[0] == ':')
	{
		// Check for Prefix
	}
	else
	{
		//Command Not Found
	}
	return (0);
}

int    Server::messageHandler(std::string messages, int clientSocket, Client &client)
{
    std::vector<Msg>     msgs;
    
	// std::cout << "Message Handler" << std::endl;
    this->makeMessages(msgs, messages);

	for (int i = 0; i < msgs.size(); i++)
	{
		if (this->commandSelector(msgs[i], clientSocket, client) == 1)
		{
			return (1);
			//TO DO. If error etc...
			return (1);
		}
	}
	return (0);
}