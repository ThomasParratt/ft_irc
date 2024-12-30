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
	printMsg(msg); //debug
	std::vector<std::string> channels = split(msg.parameters[0], ",");
	// std::cout << "printing channel names" << std::endl;
	printArray(channels);
	for (int i = 0; i < channels.size(); i++)
	{
		bool channelExists = false;
		// std::cout << "first loop: " << i << std::endl;
		for (auto &channel : channel_names)
		{
			// std::cout << "second loop: " << i << std::endl;
			if (channel.name == channels[i])
			{
				channelExists = true;
				for (auto &users : channel.channel_users)
				{
					// std::cout << "third loop: " << i << std::endl;
					if (users.nickname == client.getNickname())
					{
						int j = getChannelIndex(channels[i], channel_names);
						std::string part;
						if (!msg.trailing_msg.empty())
							part = ":" + client.getNickname() + " PART " + channel.name + " :" + msg.trailing_msg + "\r\n";
						else
							part = ":" + client.getNickname() + " PART " + channel.name + "\r\n";
						broadcastToChannel(channel_names[j], part);
						// printChannels();
						std::cout << "removing user: " << client.getNickname() << " from channel: " << channels[i] << std::endl;
						removeUser(client.getNickname(), channels[i], " has left"); // it didn't send it in libera chat
						client.leaveChannel(channels[i]); // probably don't need but just in case
						// printChannels();
						break;
					}
				}
				break;
			}
		}
		if (!channelExists)
		{
			std::cout << "channel names: " << channels[i] << std::endl;
			std::string notice = channels[i] + ": No such channel\r\n";
			send(clientSocket, notice.c_str(), notice.size(), 0);
		}
	}
	for ( auto &channel : channel_names)
	{
		if (channel.channel_users.size() == 0)
		{
			//remove channel
			int i = getChannelIndex(channel.name, channel_names);
			channel_names.erase(channel_names.begin() + i);
		}
	}
	return (0);
}