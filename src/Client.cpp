#include "Client.hpp"


Client::Client(int socket, const std::string& password) : 
					_password			(password),
					_passwordChecked	(false),
					_welcomeSent		(false),
					_socket				(socket)
{
	// setPrefix();
};

std::string Client::getNickname()
{
    return(_nickname);
}

std::string Client::getUsername()
{
	return(_username);
}

std::string Client::getPassword()
{
    return(_password);
}

std::string Client::getHost()
{
    return(_host);
}

std::string Client::getRealname()
{
    return(_realname);
}

std::string Client::getHostname()
{
    return(_hostname);
}

std::string Client::getPrefix()
{
    return _prefix;
}


bool        Client::getPasswordChecked()
{
    return(_passwordChecked);
}

bool        Client::getWelcomeSent()
{
    return(_welcomeSent);
}

bool       Client::getOperatorStatus()
{
    return(_operatorStatus);
}

int         Client::getSocket()
{
    return(_socket);
}

std::vector<std::string>& Client::getChannelsNames()
{
    return(_channelsNames);
}

void        Client::setNickname(std::string str)
{
    _nickname = str;
}

void        Client::setUsername(std::string str)
{
    _username = str;
}

void        Client::setHost(std::string str)
{
    _host = str;
}

void        Client::setHostname(std::string str)
{
    _hostname = str;
}

void        Client::setRealname(std::string str)
{
    _realname = str;
}

void       Client::setPrefix(std::string prefix)
{
    _prefix = prefix;
}

void        Client::setPasswordChecked(bool value)
{
    _passwordChecked = value;
}

void        Client::setWelcomeSent(bool value)
{
    _welcomeSent = value;
}

void        Client::setOperatorStatus(bool value)
{
    _operatorStatus = value;
}

void        Client::joinChannel(std::string channelName)
{
    /* how should we give the operator status? The first client that creates the channel should receive operatorStatus */
    std::vector<std::string>::iterator it = std::find(_channelsNames.begin(), _channelsNames.end(), channelName);
    if (it != _channelsNames.end())
        return;
    else
    {
        /* check if channel is invite only, check the channel size, check if channel requires a password*/
        _channelsNames.push_back(channelName);

    }
    std::cout << "Remaining channels:" << std::endl;
    for (const auto& channel : _channelsNames)
    {
        std::cout << "Channel: " << channel << std::endl;
    }
}

void       Client::leaveChannel(std::string channelName)
{
    std::vector<std::string>::iterator it = std::find(_channelsNames.begin(), _channelsNames.end(), channelName);
    //debug
    std::cout << "Debug: client leaving channel" << std::endl;
    if (it != _channelsNames.end())
    {
        _channelsNames.erase(it);
        //debug
        std::cout << "Debug: Client removed channel: " << channelName << std::endl;
    }
    //debug
    std::cout << "Remaining channels:" << std::endl;
    for (const auto& channel : _channelsNames)
    {
        std::cout << "Channel: " << channel << std::endl;
    }
    std::cout << "End of remaining channels" << std::endl;
}

// void    Client::leaveChannel(std::string channelName)
// {
//     auto it = std::find(_channelsNames.begin(), _channelsNames.end(), channelName);
//     std::cout << "Client removing channel: " << channelName << std::endl;

//     if (it != _channelsNames.end())
//     {
//         _channelsNames.erase(it);
//         std::cout << this->getNickname() << "'s channel removed: " << channelName << std::endl;
//     }
//     else
//     {
//         std::cout << "Channel not found: " << channelName << std::endl;
//     }

//     std::cout << "Remaining channels:" << std::endl;
//     for (const auto& channel : _channelsNames)
//     {
//         std::cout << "Channel: " << channel << std::endl;
//     }
//     std::cout << "End of remaining channels" << std::endl;
// }