#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Client.hpp"

class Msg;
// #include "Msg.hpp"

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

		void	messageHandler(std::string buffer, int clientSocket, Client &client);
		void	makeMessages(std::vector<Msg> &msgs, std::string buffer);

		int		commandSelector(Msg msg, int clientSocket, Client &client);
		int		passwordCommand(Msg msg, int clientSocket, Client &client);
		int		nicknameCommand(Msg msg, int clientSocket, Client &client);

		//Getter
		int	getServerSocket() { return _serverSocket; }
		bool getWelcomeSent() { return _welcomeSent; }
		std::string getPassword() { return _password; }
};

	int    handleMessages(char *buffer, int clientSocket, Client &client);
	std::string messageParam(char *buffer, std::string message);
