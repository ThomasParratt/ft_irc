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
        //int         clientSocket;
    public:
        Client(std::string password, bool welcomeSent);

        std::string getNickname();
        std::string getPassword();
        bool        getWelcomeSent();

        void        setNickname(std::string str);
        void        setWelcomeSent(bool value);
};
