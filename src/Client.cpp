#include "Client.hpp"


Client::Client(int socket, const std::string& password) : 
					_password			(password),
					_passwordChecked	(false),
					_welcomeSent		(false),
					_socket				(socket)
{
	//setHostIP();
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

/*std::string Client::getHostIP()
{
    return(_hostIP);
}*/

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

/*void Client::setHostIP()
{
    char hostname[NI_MAXHOST];
    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        std::cerr << "Error getting hostname" << std::endl;
        return;
    }

    struct addrinfo hints = {};
    hints.ai_family = AF_INET; // Use IPv4
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* res;
    if (getaddrinfo(hostname, nullptr, &hints, &res) != 0)
    {
        std::cerr << "Error getting address info" << std::endl;
        return ;
    }

    // Iterate through results and convert the first valid address to string
    for (struct addrinfo* p = res; p != nullptr; p = p->ai_next)
    {
        struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(p->ai_addr);
        char host_ip[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &(ipv4->sin_addr), host_ip, sizeof(host_ip)) != nullptr)
        {
            _hostIP = host_ip;
            freeaddrinfo(res); // Clean up
            return ;
        }
    }

    std::cerr << "Failed to convert address to string" << std::endl;
    freeaddrinfo(res);
}*/

/*
joinChannel -

the client will loop through their list of channels and check if the channel is already in the list. If it is, it will return. If it is not, it will add the channel to the list.

leaveChannel -
the client will loop through their list of channels and check if the channel is in the list. If it is, it will remove the channel from the list.
*/

void        Client::joinChannel(std::string channelName)
{
    std::vector<std::string>::iterator it = std::find(_channelsNames.begin(), _channelsNames.end(), channelName);
    if (it != _channelsNames.end())
        return;
    else
    {
        /* check if channel is invite only, check the channel size, check if channel requires a password*/
        _channelsNames.push_back(channelName);

    }
    // //debug
    // std::cout << "Remaining channels:" << std::endl;
    // for (const auto& channel : _channelsNames)
    // {
    //     std::cout << "Channel: " << channel << std::endl;
    // }
}

void       Client::leaveChannel(std::string channelName)
{
    std::vector<std::string>::iterator it = std::find(_channelsNames.begin(), _channelsNames.end(), channelName);
    if (it != _channelsNames.end())
    {
        _channelsNames.erase(it);
    }
    //debug
    // for (const auto& channel : _channelsNames)
    // {
    //     std::cout << "Channel: " << channel << std::endl;
    // }
    // std::cout << "End of remaining channels" << std::endl;
}
