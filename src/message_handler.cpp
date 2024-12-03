
#include "Msg.hpp"
#include "Server.hpp"


#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#include <vector> //

// std::string		getCommand(std::vector<std::string> array)
// {
// 	if (array[0][0] != ':')
// 	{
// 		// std::cout << "Command: " << array[0] << std::endl;
// 		return (array[0]);
// 	}
// 	else
// 	{
// 		// std::cout << "Command: " << array[1] << std::endl;
// 		return (array[1]);
// 	}
// }



// /*
// 	Returns the index of 1st parameter. 
// 	Return -1, if no parameter found.
// */
// int	findParamterIndex(std::vector<std::string> array)	//count params
// {
// 	int i = 0;
// 	if (array[i][0] == ':')
// 	{
// 		i++;				//Skip Prefix
// 	}
// 	i++;					//Skip command

// 	if (i == array.size())	//Vector Boundary Check
// 	{
// 		return (-1);
// 	}

// 	if (array[i][0] != ':')	
// 	{
// 		return (i);			//There is a parameter
// 	}
// 	else
// 	{
// 		return (-1);		// No paramters found
// 	}

// }


// std::vector<std::string>	getParameters(std::vector<std::string> array)
// {
// 	std::vector<std::string> parameters;
	
// 	int i;
// 	if ((i = findParamterIndex(array)) == -1)
// 	{
// 		return (parameters);
// 	}

// 	for (i; i < array.size(); i++)
// 	{
// 		if (array[i][0] != ':')
// 		{
// 			// std::cout << "Params: " << array[i] << std::endl;
// 			parameters.push_back(array[i]);
// 		}
//     }	
// 	return (parameters);
// }

// std::string getTrailingMessage(std::vector<std::string> array)
// {
// 	size_t last_string = array.size() - 1;

// 	if (array[last_string][0] == ':')
// 	{
// 		// std::cout << "Trailing Message: " << &(array[last_string][1]) << std::endl;		
// 		return (&(array[last_string][1]));	// get rid of :
// 	}
// 	else
// 	{
// 		return ("");
// 	}
// }


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
    int count = 0;										//Del

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

	removeStringEnd(message);
	std::cout << "message: " << message  << std::endl;

	//Array gets rid of spaces between Words (except for spaces within trailing msg)
	stringToArray(message, message_array);
	printArray(message_array);

    initializeMsg(msg, message_array);
	printMsg(msg);

	return ;
}

void	Server::makeMessages(std::vector<Msg> &msgs, std::string buffer)
{
	Msg				msg;

	std::string		single_msg;
    size_t			start = 0;
	size_t			end = 0;
	size_t			length = 0;

	while ((end = buffer.find("\n", start)) != std::string::npos)
	{
		length = end - start;
		single_msg = buffer.substr(start, length);
		std::cout << "single_msg: " << single_msg << std::endl;

		makeMsgfromString(msg, single_msg);
		msgs.push_back(msg);
		start = end + 1;
	}
}

void    Server::messageHandler(std::string messages)
{
    std::vector<Msg>     msgs;
    
    this->makeMessages(msgs, messages);


    /*
        To Do:
        1. Commmand Selector 
        2.    -> Run Command
    */

}