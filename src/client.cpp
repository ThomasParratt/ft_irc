#include "client.hpp"

Client::Client(std::string password, bool welcomeSent) : password(password), welcomeSent(welcomeSent) {};

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

void        Client::setNickname(std::string str)
{
    nickname = str;
}

void        Client::setWelcomeSent(bool value)
{
    welcomeSent = value;
}
