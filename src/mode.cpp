
#include "Server.hpp"

int		Server::modeCommand(Msg msg, int clientSocket, Client &client)
{
	// std::cout << "Mode Command" << std::endl;

	if (msg.parameters.size() <= 1)
	{
		std::cout << "Error: Not enough parameter" << std::endl;
	}
	else if (msg.parameters[0][0] != '#')
	{
		std::cout << "Error: Not a Channel" << std::endl;
	}
	else if (msg.parameters[1].empty())
	{
		std::cout << "Error: No Mode Parameter Given" << std::endl;		
	}
	else if (msg.parameters[1] == "+i" || msg.parameters[1] == "-i")
	{
		// TODO: Set/remove Invite-only channel
	}
	else if (msg.parameters[1] == "+t" || msg.parameters[1] == "-t")
	{
		//TODO: Set/remove the restrictions of the TOPIC command to channel operators
	}
	else if (msg.parameters[1] == "+k" || msg.parameters[1] == "-k")
	{
		//TODO: Set/remove the channel key (password)
	}
	else if (msg.parameters[1] == "+o" || msg.parameters[1] == "-o")
	{
		//TODO: Give/take channel operator privilege
	}
	else if (msg.parameters[1] == "+l" || msg.parameters[1] == "-l")
	{
		//TODO: Set/remove the user limit to channel
	}
	else
	{
		//Error: Unknown Parameter
	}
	return (0);
}
