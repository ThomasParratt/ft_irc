#include "Server.hpp"
#include "Msg.hpp"

// added split because I realized if we input /part #chn3,#chn4 it will not work
//para[0] = #chn3,#chn4 instead of para[0] = #chn3 & para[1] = #chn4
std::vector<std::string> split(std::string str, std::string token)
{
	std::vector<std::string>result;
	while (str.size())
	{
		int index = str.find(token);
		if (index != std::string::npos)
		{
			result.push_back(str.substr(0, index));
			str = str.substr(index + token.size());
			if (str.size() == 0) result.push_back(str);
		}
		else
		{
			result.push_back(str);
			str = "";
		}
	}
	return result;
}

int		Server::partCommand(Msg msg, int clientSocket, Client &client)
{
	// std::cout << "PART Command" << std::endl;
	// printMsg(msg); //debug
	std::vector<std::string> channels = split(msg.parameters[0], ",");
	// std::cout << "printing channel names" << std::endl;
	// printArray(channels);
	if (channels.size() == 0)
	{
		std::string notice = ":ircserv 461 " + client.getNickname() + " PART :Not enough parameters\r\n";
		send(clientSocket, notice.c_str(), notice.size(), 0);
		return (1);
	}
	for (int i = 0; i < channels.size(); i++)
	{
		bool channelExists = false;
		for (auto &channel : _channel_names)
		{
			if (channel.name == channels[i])
			{
				channelExists = true;
				if (userExists(client.getNickname(), channels[i]))
				{
					int j = getChannelIndex(channels[i], _channel_names);
					std::string fullPrefix = client.getNickname() + "!" + "~" + client.getUsername() + "@" + client.getHostIP();
					std::string part;
					if (!msg.trailing_msg.empty())
						part = ":" + fullPrefix + " PART " + channel.name + " :" + msg.trailing_msg + "\r\n";
					else
						part = ":" + fullPrefix + " PART " + channel.name + "\r\n";
					// std::cout << "debug part channel before:" << std::endl;
					// printChannelUsers(channel_names[j]);
					// std::cout << "removing user: " << client.getNickname() << " from channel: " << channels[i] << std::endl;
					removeUser(client.getNickname(), channels[i], part, 0);
					client.leaveChannel(channels[i]);
					broadcastToChannel(_channel_names[j], part, client, 0);
					// printChannelUsers(channel_names[j]);
					break;
				} else {
					std::string notice = ":ircserv 442 " + client.getNickname() + " " + channels[i] + " :You're not on that channel\r\n";
					send(clientSocket, notice.c_str(), notice.size(), 0);
				}
				break;
			}
		}
		if (!channelExists)
		{
			std::string notice = ":ircserv 403 " + client.getNickname() + " " + channels[i] + " :No such channel\r\n";
			send(clientSocket, notice.c_str(), notice.size(), 0);
		}
	}
	for ( auto &channel : _channel_names)
	{
		if (channel.channel_users.size() == 0)
		{
			//remove channel
			// std::cout << "Removing: " << channel.name << " :no more users"<<std::endl;
			int i = getChannelIndex(channel.name, _channel_names);
			_channel_names.erase(_channel_names.begin() + i);
		}
	}
	return (0);
}