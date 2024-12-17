#include "Client.hpp"

Client::Client(int socket, const std::string& password) : socket(socket), password(password), welcomeSent(false) 
{
    setHostIP();
};

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

std::string Client::getHostIP()
{
    return(hostIP);
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

void Client::setHostIP()
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
            hostIP = host_ip;
            freeaddrinfo(res); // Clean up
            return ;
        }
    }

    std::cerr << "Failed to convert address to string" << std::endl;
    freeaddrinfo(res);
}


void        Client::setWelcomeSent(bool value)
{
    welcomeSent = value;
}

void        Client::setOperatorStatus(bool value)
{
    operatorStatus = value;
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