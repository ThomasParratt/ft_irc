#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Client.hpp"

class Server {
	private:
		std::string _password;
		std::string _nickname;
		sockaddr_in _serverAddr;

		bool _welcomeSent;
		int _port;
		int _serverSocket;
	public:
		Server(std::string password, int port);
		~Server();
		int serverInit();
		void serverLoop();
		// int acceptClient(std::vector<pollfd>& pollfds);

		//Getter
		int getServerSocket() { return _serverSocket; }
		bool getWelcomeSent() { return _welcomeSent; }
		std::string getPassword() { return _password; }
};

	int    handleMessages(char *buffer, int clientSocket, Client &client);
	std::string messageParam(char *buffer, std::string message);
