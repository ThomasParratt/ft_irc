#include <iostream>
#pragma once

class Client
{
    private:
        std::string nickname;
        std::string username;
        std::string hostname;
        std::string servername;
        std::string realname;
        std::string password;
        bool        welcomeSent;
        int         socket;
    public:
        Client(int socket, const std::string& password);

        std::string getNickname();
        std::string getPassword();
        bool        getWelcomeSent();
        int         getSocket();

        void        setNickname(std::string str);
        void        setWelcomeSent(bool value);
};