#pragma once

#include <iostream>
#include <vector>

class Client
{
    private:
        std::string nickname;
        std::string username;
        std::string hostname;
        std::string servername;
        std::string realname;
        std::string password;

        std::vector<std::string> channelsNames;
        bool        welcomeSent;
        int         socket;
    public:
        Client(int socket, const std::string& password);

        std::string getNickname();
        std::string getPassword();
        std::vector<std::string>& getChannelsNames();
        bool        getWelcomeSent();
        int         getSocket();

        void        setNickname(std::string str);
        void        setWelcomeSent(bool value);
        void        joinChannel(std::string channelName);
        void        leaveChannel(std::string channelName);
};