
#include "Server.hpp"

void	Server::printChannelUsers(Channel channel)
{
	for (int i = 0; i < channel.channel_users.size(); i++)
	{
		std::cout << "User[" << i << "]: " <<  channel.channel_users[i].nickname  << " (op = " <<  channel.channel_users[i].operator_permissions << ")" << std::endl;
	}	
}

void	Server::addChannelUser(Channel &channel, Client &client, bool operator_permissions)
{
	//Make new Channel User
	User	new_user;
	new_user.nickname = client.getNickname();
	new_user.operator_permissions = operator_permissions;

	//Add into channel Users
	channel.channel_users.push_back(new_user);
}

int		Server::createChannel(Msg msg, int clientSocket, Client &client)
{
	Channel new_channel(msg.parameters[0]);

	addChannelUser(new_channel, client, true);

	this->channel_names.push_back(new_channel);

	std::string response = ":ircserver PRIVMSG " + msg.parameters[0] + " :Welcome to the Channel " + msg.parameters[0] + "\r\n";	


	send(clientSocket, response.c_str(), response.size(), 0);

	/*
		TODO: Send more messages to Irssi f.ex. tell Irssi who is operator -> Check Manual
	*/


	return (0);
}

void		Server::printChannels()
{
	for (int i = 0; i < this->channel_names.size(); i++)
	{
		std::cout << "Channel[" << i << "]: "<< channel_names[i].name << std::endl;
		printChannelUsers(channel_names[i]);
	}
}

/*
	Returns index of channel in vector.
	- Returns 0 or greater, if channel exists.
	- Returns -1, if channel doesn't exist. 
*/
int		getChannelIndex(std::string channel_name, std::vector<Channel> channel_names)
{
	int i;

	for (i = 0; i < channel_names.size(); i++)
	{
		if (channel_name == channel_names[i].name)
		{
			return (i);
		}
	}
	return (-1);
}

int		Channel::getNumberOfChannelUsers()
{
	std::vector<User> channel_users = this -> getChannelUsers();

	int num_of_channel_users  = channel_users.size();

	return (num_of_channel_users);
}
/*
	Returns true, if Number of Channel Users equals or exceeds Channel User Limit.
	Returns false,
		i if that is not the case
		ii. if no Channel Limit set.
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

bool		Channel::doesChannelHavePassword()
{
	if (channel_key == "")
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

	if (channel.isChannelInviteOnly() == true)
	{
		std::cout << "INVITE ONLY" << std::endl;
		/*
			a. If Invited
				-> OK

			b. If not Invited
				-> Send message
		*/

		for (auto &it : channel.invited)
		{
			if (it == client.getNickname())
			{
				std::cout << "Invited user = " << it << std::endl;
				std::cout << "ZERO" << std::endl;
				return (0);
			}
		}

		// YOU SHOULD'VE BEEN INVITED. 
		message  = ":ircserv 473 " + client.getNickname() + " " + msg.parameters[0] + " :Cannot join channel (+i) - you must be invited\r\n";
		send(clientSocket, message.c_str(), message.size(), 0);

		//example << JOIN #BANNNANAss
		//example >> :lithium.libera.chat 473 gravity123 #BANNNANAss :Cannot join channel (+i) - you must be invited
		std::cout << "ONE" << std::endl;
		return (1);
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
		if (channel.getChannelKey() == msg.parameters[1]) 	// Password Correct
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
	return (0);
}

int		Server::joinChannel(Msg msg, int clientSocket, Client &client)
{
	int index = getChannelIndex(msg.parameters[0], this->channel_names);
	if (index == -1)
	{
		//Channel doesn't exist
	}

	if (channelJoinChecks(this -> channel_names[index], msg, clientSocket, client) != 0)
	{
		return (1);
	}
	std::cout << "ADDING CHANNEL USER" << std::endl;
	addChannelUser(this->channel_names[index], client, false);

	return (0);
}

void	Server::broadcastToChannel(Channel &channel, std::string message)
{
	std::vector<User> users;
	int socket;

	users = channel.getChannelUsers();

	for (int i = 0; i < users.size(); i++)
	{
		int socket = getClientSocket(users[i].nickname);
		if (socket != -2)
		{
			send(socket, message.c_str(), message.size(), 0);
		}
	}
}

