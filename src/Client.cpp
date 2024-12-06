#include "Client.hpp"

Client::Client(int socket, const std::string& password) : socket(socket), password(password), welcomeSent(false) {};

std::string Client::getNickname()
{
    return(nickname);
}

std::string Client::getPassword()
{
    return(password);
}

bool        Client::getWelcomeSent()
{
    return(welcomeSent);
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

void        Client::setWelcomeSent(bool value)
{
    welcomeSent = value;
}

void        Client::joinChannel(std::string channelName)
{
    std::vector<std::string>::iterator it = std::find(channelsNames.begin(), channelsNames.end(), channelName);
    if (it != channelsNames.end())
        return;
    else
        channelsNames.push_back(channelName);
}

void       Client::leaveChannel(std::string channelName)
{
    std::vector<std::string>::iterator it = std::find(channelsNames.begin(), channelsNames.end(), channelName);
    if (it != channelsNames.end())
        channelsNames.erase(it);
}