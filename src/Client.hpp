#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <unistd.h>

class Client
{
    private:
        std::string nickname;
        std::string username;
        std::string hostIP;
        std::string realname;
        std::string password;

        std::vector<std::string> channelsNames;
        bool        welcomeSent;
        bool        operatorStatus;
        int         socket;
    public:
        Client(int socket, const std::string& password);

        std::string getNickname();
        std::string getUsername();
        std::string getPassword();
        std::vector<std::string>& getChannelsNames();
        bool        getWelcomeSent();
        bool        getOperatorStatus();
        int         getSocket();
        std::string getHostIP();

        void        setNickname(std::string str);
        void        setUsername(std::string str);
        void        setHostIP();
        void        setWelcomeSent(bool value);
        void        setOperatorStatus(bool value);
        void        joinChannel(std::string channelName);
        void        leaveChannel(std::string channelName);  
};