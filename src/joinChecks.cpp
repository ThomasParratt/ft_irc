
#include "Server.hpp"

int		Channel::getNumberOfChannelOperators()			// Delete? - function not used anywhere.
{
	std::vector<User> channel_users = this -> getChannelUsers();

	int num_of_channel_operators = 0;

	for (size_t i = 0; i < channel_users.size(); i++)
	{
		if (channel_users[i].operator_permissions == true)
		{
			num_of_channel_operators++;
		}
	}
	return (num_of_channel_operators);
}

int		Channel::getNumberOfChannelUsers()
{
	std::vector<User>	channel_users;
	int					num_of_channel_users;

	channel_users 			= this -> getChannelUsers();
	num_of_channel_users 	= channel_users.size();

	return (num_of_channel_users);
}

bool		Channel::doesChannelHavePassword()
{
	if (this->_keyRequired == false)
	{
		return (false);
	}
	else
	{
		return (true);
	}
}

/*
	Returns true, if Number of Channel Users equals or exceeds Channel User Limit.
	Returns false,
		i if that is not the case
		ii. if no Channel Limit set (i.e. user_limit = -1).
*/
bool		Channel::isChannelFull()
{
	int user_limit 				= this -> getUserLimit();
	int	num_of_channel_users 	= this -> getNumberOfChannelUsers();

	if (user_limit == -1 || user_limit > num_of_channel_users)
	{
		return (false);
	}
	else
	{
		return (true);
	}
}

int		Server::channelJoinChecks(Channel channel, Msg msg, Client &client)
{
	std::string message;
	
	// checkIfChannelFull()

	if (channel.isChannelFull() == true)
	{
		message = ":ircserv 471 " + client.getNickname() +  " " + msg.parameters[0] + " :Cannot join channel (+l) - channel is full, try again later\r\n";
		
		send(client.getSocket(), message.c_str(), message.size(), 0);
		return (1) ;
	}

	if (channel.doesChannelHavePassword() == true)
	{
		if (msg.parameters.size() <= 1)						 //No password parameter passed.
		{
			message = ":ircserv 475 " + client.getNickname() + " " + msg.parameters[0] + " :Cannot join channel (+k) - no password entered\r\n";
			send(client.getSocket(), message.c_str(), message.size(), 0);
			return (1);
		}

		std::string 	password = msg.parameters[1];
		// std::cout << "Password: " << password << std::endl;						//debug
		// std::cout << "Channel Key: " << channel.getChannelKey() << std::endl;	//debug
		if (channel.getChannelKey() == password) 			//Password Correct
		{

		}
		else 												//Password Incorrect
		{
			message = ":ircserv 475 " + client.getNickname() + " " + msg.parameters[0] + " :Cannot join channel (+k) - password incorrect\r\n";
			send(client.getSocket(), message.c_str(), message.size(), 0);
			return (1);
		}
	}

	if (channel.isChannelInviteOnly() == true)
	{

		for (auto &it : channel.getInvitedList())
		{
			if (it == client.getNickname())
			{
				return (0);				//User was Invited
			}
		}

		message  = ":ircserv 473 " + client.getNickname() + " " + msg.parameters[0] + " :Cannot join channel (+i) - you must be invited\r\n";
		send(client.getSocket(), message.c_str(), message.size(), 0);

		return (1);						//User was NOT Invited
	}
	return (0);
}
