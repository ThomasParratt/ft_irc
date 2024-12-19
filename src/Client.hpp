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
        std::string _nickname;
        std::string _username;
        std::string _hostIP;
        std::string _realname;
        std::string _password;

        std::vector<std::string> _channelsNames;
        bool        _passwordChecked;
        bool        _welcomeSent;
        bool        _operatorStatus;
        int         _socket;
    public:
        Client(int socket, const std::string& password);

        std::string getNickname();
        std::string getUsername();
        std::string getPassword();
        std::vector<std::string>& getChannelsNames();
        bool        getPasswordChecked();
        bool        getWelcomeSent();
        bool        getOperatorStatus();
        int         getSocket();
        std::string getHostIP();

        void        setNickname(std::string str);
        void        setUsername(std::string str);
        void        setHostIP();
        void        setPasswordChecked(bool value);
        void        setWelcomeSent(bool value);
        void        setOperatorStatus(bool value);
        void        joinChannel(std::string channelName);
        void        leaveChannel(std::string channelName);  
};