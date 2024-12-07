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

#include "Client.hpp"

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
		//std::map<std::string, std::vector<client>> _channels; Map of channel names to the lists of clients in each channel
	public:
		Server(std::string password, int port);
		~Server();
		int serverInit();
		int setServHostName();
		// int acceptClient(std::vector<pollfd>& pollfds);
		void serverLoop();
		// void boardcastMessage(const std::string& message, const std::string& channelName, int senderSocket);

		void	messageHandler(std::string buffer, int clientSocket, Client &client);
		void	makeMessages(std::vector<Msg> &msgs, std::string buffer);

		int		commandSelector(Msg msg, int clientSocket, Client &client);
		int		passwordCommand(Msg msg, int clientSocket, Client &client);
		int		nicknameCommand(Msg msg, int clientSocket, Client &client);

		//Getter
		int	getServerSocket() { return _serverSocket; }
		bool getWelcomeSent() { return _welcomeSent; }
		std::string getPassword() { return _password; }
		std::string getServHostName() { return _servHostName; }
};

	int    handleMessages(char *buffer, int clientSocket, Client &client);
	std::string messageParam(char *buffer, std::string message);
	std::string getCurrentTime();
