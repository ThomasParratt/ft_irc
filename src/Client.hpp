#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <string.h>

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
        bool        operatorStatus;
        int         socket;
    public:
        Client(int socket, const std::string& password);

        std::string getNickname();
        std::string getPassword();
        std::vector<std::string>& getChannelsNames();
        bool        getWelcomeSent();
        bool        getOperatorStatus();
        int         getSocket();

        void        setNickname(std::string str);
        void        setWelcomeSent(bool value);
        void        joinChannel(std::string channelName);
        void        leaveChannel(std::string channelName);
};