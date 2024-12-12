#include "Server.hpp"
#include "Msg.hpp"

/* I am running into an issue where the client that left can still send messages, however, it is not receiving message*/

int		Server::partCommand(Msg msg, int clientSocket, Client &client)
{
	std::cout << "PART Command" << std::endl;
	for (auto &channel : channel_names)
	{
		if (channel.name == msg.parameters[0])
		{
			for (auto &users : channel.channel_users)
			{
				if (users.nickname == client.getNickname())
				{
					int i = getChannelIndex(msg.parameters[0], channel_names);
					std::string part = ":" + client.getNickname() + " PART " + channel.name + "\r\n";
					broadcastToChannel(channel_names[i], part);
					printChannels();
					removeUser(client.getNickname(), msg.parameters[0], "has left");
					client.leaveChannel(msg.parameters[0]); // probably don't need but just in case
					std::cout << "USER Removed" << std::endl;
					printChannels();
					return (0);
				}
			}
		}
	}
	//TO DO: Send Message to Channel
	//TO DO: Send Message to User
	// check channel user size if size = 0 then remove channel
	return (0);
}