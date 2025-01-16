#include "Server.hpp"
#include "Msg.hpp"

void	Server::whoCommand(Msg msg, Client &client)
{
    int i = getChannelIndex(msg.parameters[0], _channel_names);
    const std::vector<User>& channel_users = _channel_names[i].getChannelUsers();

    for (size_t i = 0; i < channel_users.size(); ++i) 
	{
        for (auto it : _clients)
        {
            if (it.getNickname() == channel_users[i].nickname)
            {
                std::string message = ":ircserver 352 " + client.getNickname() + " " + msg.parameters[0] + " " + it.getUsername() + " " + it.getHostname() + " ircserver " + it.getNickname() + " :" + it.getRealname() + "\r\n";
                send(client.getSocket(), message.c_str(), message.size(), 0);
            }
        }
    }
    std::string message = ":ircserver 315 " + client.getNickname() + " " + msg.parameters[0] + " :End of /WHO list\r\n";
    send(client.getSocket(), message.c_str(), message.size(), 0);
}

// username hostname server nickname flags hopCount realName
