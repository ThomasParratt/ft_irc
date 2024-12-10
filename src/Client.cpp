#include "Client.hpp"

Client::Client(int socket, const std::string& password) : socket(socket), password(password), welcomeSent(false) {};

std::string Client::getNickname()
{
    return(nickname);
}

std::string Client::getUsername()
{
    return(username);
}

std::string Client::getPassword()
{
    return(password);
}

bool        Client::getWelcomeSent()
{
    return(welcomeSent);
}

bool       Client::getOperatorStatus()
{
    return(operatorStatus);
}

int         Client::getSocket()
{
    return(socket);
}

std::vector<std::string>& Client::getChannelsNames()
{
    return(channelsNames);
}

void        Client::setNickname(std::string str)
{
    nickname = str;
}

void        Client::setUsername(std::string str)
{
    username = str;
}

void        Client::setWelcomeSent(bool value)
{
    welcomeSent = value;
}

void        Client::joinChannel(std::string channelName)
{
    /* how should we give the operator status? The first client that creates the channel should receive operatorStatus */
    std::vector<std::string>::iterator it = std::find(channelsNames.begin(), channelsNames.end(), channelName);
    if (it != channelsNames.end())
        return;
    else
    {
        /* check if channel is invite only, check the channel size, check if channel requires a password*/
        channelsNames.push_back(channelName);

    }
}

void       Client::leaveChannel(std::string channelName)
{
    std::vector<std::string>::iterator it = std::find(channelsNames.begin(), channelsNames.end(), channelName);
    if (it != channelsNames.end())
        channelsNames.erase(it);
}