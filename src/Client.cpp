#include "Client.hpp"

Client::Client(int socket, const std::string& password) : socket(socket), password(password), welcomeSent(welcomeSent) {};

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

void        Client::setNickname(std::string str)
{
    nickname = str;
}

void        Client::setWelcomeSent(bool value)
{
    welcomeSent = value;
}