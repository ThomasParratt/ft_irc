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

//functions definitions
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
			// std::cout << "Params[" << i << "]: " << array[i] << std::endl;
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
		return (&(array[last_string][1]));			//get rid of ':' character
	}
	else
	{
		return ("");
	}
}

int		Server::joinCommand(Msg msg, int clientSocket, Client &client)
{
	int i = getChannelIndex(msg.parameters[0], this->channel_names);
	if (i == -1)
	{
		// std::cout << "No Existing Channel Found" << std::endl;
		createChannel(msg, clientSocket, client);
	}
	else
	{ 
		// std::cout << "Channel Found at i = " << i << std::endl;
		joinChannel(msg, clientSocket, client);// If fail - then leave???
	}
	i = getChannelIndex(msg.parameters[0], this->channel_names);

	std::string message = ":ircserver PRIVMSG " + msg.parameters[0] + " :" + client.getNickname() + " has joined " + msg.parameters[0] + "\r\n";	

	broadcastToChannel(this->channel_names[i], message);//Send sender Fd??

	//WELCOME_MSG - Send message to client who connected to channel

	/*
		Send This to Server!
			:sender_nickname!user@host PRIVMSG #channel_name :message_text
			:Alice!alice@irc.example.com PRIVMSG #general :Hello everyone!
	*/

	// printChannels();
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
		Server::userCommand(msg, clientSocket, client);
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
		std::cout << "debug 1" << std::endl;
		topicCommand(msg, clientSocket, client);
	}
	else if  (msg.command == "MODE")
	{
		//modeCommand(msg, clientSocket, client);
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