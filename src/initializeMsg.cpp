
#include "Server.hpp"

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
		return (&(array[last_string][1]));			//gets rid of ':' character
	}
	else
	{
		return ("");
	}
}

void	initializeMsg(Msg &msg, std::vector<std::string> array)
{	
	msg.command = getCommand(array);
	msg.parameters = getParameters(array);
	msg.trailing_msg = getTrailingMessage(array);
}


int     count_msgs(std::string buffer)		//Function not used anywhere - delete?
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

int	count_words(std::string buffer)		//Function not used anywhere - delete?
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
