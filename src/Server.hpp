#pragma once

#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <mutex>

#include "Client.hpp"
#include "Channel.hpp"
#include "Msg.hpp"

#define MAX_LEN_HOSTNAME 64

extern bool servRunning;

class Msg;
// #include "Msg.hpp"

class Server {
	private:
		std::string _password;
		std::string _nickname;
		std::string _servHostName;
		sockaddr_in _serverAddr;

		bool _welcomeSent;
		int _port;
		int _serverSocket;
		time_t _startTime;
		std::string _startTimeStr;

		std::vector<Client> clients;
		std::vector<Channel> channel_names;
		std::vector<pollfd> pollfds;

	public:
		Server(std::string password, int port);
		~Server();
		int serverInit();
		int setServHostName();
		void acceptClient();
		void serverLoop();

		int		messageHandler(std::string buffer, int clientSocket, Client &client);
		void	makeMessages(std::vector<Msg> &msgs, std::string buffer);

		int		commandSelector(Msg msg, int clientSocket, Client &client);
		int		passwordCommand(Msg msg, int clientSocket, Client &client);
		int		nicknameCommand(Msg msg, int clientSocket, Client &client);
		int		kickCommand(Msg msg, int clientSocket, Client &client);
		int		userExists(std::string user, std::string channel);
		int		removeUser(std::string user, std::string channel, std::string message);

		int     topicCommand(Msg msg, int clientSocket, Client &client);
		void	topicPrint(Msg msg, int clientSocket, Client &client);
		int		partCommand(Msg msg, int clientSocket, Client &client);
		int		joinCommand(Msg msg, int clientSocket, Client &client);
		int     createChannel(Msg msg, int clientSocket, Client &client);
		int		joinChannel(Msg msg, int clientSocket, Client &client);
		void	addChannelUser(Channel &new_channel, Client &client, bool operator_permissions);
		void	broadcastToChannel(Channel &channel, std::string message);
		int		getClientSocket(std::string nickname);

		void	userMessageToChannel(Channel channel, int sender_socket, std::string message);
		int		privmsgCommand(Msg msg, int clientSocket, Client &client);
		void	channelMessage(Msg msg, int clientSocket, Client &client);		
		void	directMessage(Msg msg, int clientSocket, Client &client);

		void	printChannels();
		void	printChannelUsers(Channel channel);
  		int		operCommand(Msg msg, int clientSocket, Client &client);

		int		clientLoop(const std::string& nickname); //added this
		

		//Getter
		int	getServerSocket() { return _serverSocket; }
		bool getWelcomeSent() { return _welcomeSent; }
		std::string getPassword() { return _password; }
		std::string getServHostName() { return _servHostName; }
		std::string getStartTimeStr() { return _startTimeStr; }
};

	std::string getCurrentTime();
	int		getChannelIndex(std::string channel_name, std::vector<Channel> channel_names);
	void	printMsg(Msg msg);