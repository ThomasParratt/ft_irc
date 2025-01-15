
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

// bool		Channel::doesChannelHavePassword()
// {
// 	if (_channel_key == "")
// 	{
// 		return (false);
// 	}
// 	else
// 	{
// 		return (true);
// 	}
// }

// Maybe we should just check key with _keyRequired, because what if the key is empty, although it shouldn't happen.
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

int		Server::channelJoinChecks(Channel channel, Msg msg, int clientSocket, Client &client)
{
	std::string message;
	
	if (channel.isChannelFull() == true)
	{
		message = ":ircserv 471 " + client.getNickname() +  " " + msg.parameters[0] + " :Cannot join channel (+l) - channel is full, try again later\r\n";
		
		send(clientSocket, message.c_str(), message.size(), 0);

		//Example - 14:13 -!- Cannot join to channel #ABC456 (Channel is full)
		//Example - :copper.libera.chat 471 mkorpela_ #ABC567 :Cannot join channel (+l) - channel is full, try again later		
		return (1) ;
	}

	if (channel.doesChannelHavePassword() == true)
	{
		if (msg.parameters.size() <= 1)						 //No password parameter passed.
		{
			message = ":ircserv 475 " + client.getNickname() + " " + msg.parameters[0] + " :Cannot join channel (+k) - no password entered\r\n"; // Trailing messages don't print
			send(clientSocket, message.c_str(), message.size(), 0);

			// << JOIN #ABC1234
			// >> :molybdenum.libera.chat 475 apppleess #ABC1234 :Cannot join channel (+k) - bad key

			return (1);
		}

		std::string 	password = msg.parameters[1];
		std::cout << "Password: " << password << std::endl;
		std::cout << "Channel Key: " << channel.getChannelKey() << std::endl;
		if (channel.getChannelKey() == password) 			//Password Correct
		{

			// message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + this->getServHostName() + " JOIN " + msg.parameters[0] + "\r\n";
			// std::cout << "message: " << message;
			// send(clientSocket, message.c_str(), message.size(), 0); //Send MESSAGE here if needed... I'll think about it later. :)

			// << JOIN #ABC1234 123456
			// >> :apppleess!~mkorpela@194.136.126.52 JOIN #ABC1234
		}
		else 												//Password Incorrect
		{
			message = ":ircserv 475 " + client.getNickname() + " " + msg.parameters[0] + " :Cannot join channel (+k) - password incorrect\r\n"; // Trailing messages don't print
			send(clientSocket, message.c_str(), message.size(), 0);

			// << JOIN #ABC1234 lol
			// >> :molybdenum.libera.chat 475 apppleess #ABC1234 :Cannot join channel (+k) - bad key

			return (1);
		}
	}

	if (channel.isChannelInviteOnly() == true)
	{
		/*
			a. If Invited
				-> OK

			b. If not Invited
				-> Send message
		*/

		for (auto &it : channel.getInvitedList())
		{
			if (it == client.getNickname())
			{
				return (0);				//User was Invited
			}
		}

		message  = ":ircserv 473 " + client.getNickname() + " " + msg.parameters[0] + " :Cannot join channel (+i) - you must be invited\r\n";
		send(clientSocket, message.c_str(), message.size(), 0);

		//example << JOIN #BANNNANAss
		//example >> :lithium.libera.chat 473 gravity123 #BANNNANAss :Cannot join channel (+i) - you must be invited
		return (1);						//User was NOT Invited
	}
	return (0);
}
