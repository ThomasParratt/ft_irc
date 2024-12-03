#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

	int handleMessages(char *buffer, std::string password, std::string nickname, int clientSocket, bool welcomeSent);
	std::string messageParam(char *buffer, std::string message);
