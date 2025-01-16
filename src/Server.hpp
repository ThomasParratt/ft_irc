#pragma once

#include <string.h>		//strerror()
#include <map>			//std::map()
#include <poll.h>		// pollfd
#include <csignal>		// signal(), SIGINT

#include "Channel.hpp"
#include "Msg.hpp"

#define MAX_LEN_HOSTNAME 64

extern bool servRunning;

class Server {
	private:
		std::string _password;
		std::string _nickname;
		std::string _servHostName;
		sockaddr_in _serverAddr;

		bool		_welcomeSent;
		int			_port;
		int			_serverSocket;
		time_t		_startTime;
		std::string _startTimeStr;

		std::vector<Client>		_clients;
		std::vector<Channel>	_channel_names;
		std::vector<pollfd>		_pollfds;

	public:
		Server(std::string password, int port);
		~Server();
		int		serverInit();
		int		setServHostName();
		void	acceptClient();
		void	serverLoop();

		void	removeFromAll(int i);

		int		makeSelectAndRunCommand(std::string buffer, Client &client);
		void	makeMessages(std::vector<Msg> &msgs, std::string buffer);

		int		commandSelector(Msg msg, Client &client);
		int		passwordCommand(Msg msg, Client &client);
		int		nicknameCommand(Msg msg, Client &client);
		int		userCommand(Msg msg, Client &client);
		void	kickCommand(Msg msg, Client &client);
		void	inviteCommand(Msg msg, Client &client);

		int		userExists(std::string user, std::string channel);
		int		channelExists(std::string channel);
		int		removeUser(std::string user, std::string channel, std::string message, int partOrKick);
		int		clientStatus(Msg msg, Client &client);
		Channel* getChannel(std::string channelName);

		int     topicCommand(Msg msg, Client &client);
		void	topicPrint(std::string channelName, Client &client);
		void	partCommand(Msg msg, Client &client);
		void	joinCommand(Msg msg, Client &client);
		void	whoCommand(Msg msg, Client &client);
		void    createChannel(Msg msg, Client &client);
		int		channelJoinChecks(Channel channel, Msg msg, Client &client);

		void	addChannelUser(Channel &new_channel, Client &client, bool operator_permissions);
		void	broadcastToChannel(Channel &channel, std::string message, Client &client, int check);
		int		getClientSocket(std::string nickname);

		void	userMessageToChannel(Channel channel, int sender_socket, std::string message);
		void	privmsgCommand(Msg msg, Client &client);
		void	channelMessage(Msg msg, Client &client);		
		void	directMessage(Msg msg, Client &client);
		void	joinChannelMessage(std::string channelName, Client &client);

		void	modeCommand(Msg msg, Client &client);
		int		channelChecks(Msg msg, Client &client);
		void	topicMode(Msg msg, Client &client, Channel* tarChannel);
		void	keyMode(Msg msg, Client &client, Channel* tarChannel);
		void	inviteMode(Msg msg, Client &client, Channel* tarChannel);
		void	operatorMode(Msg msg, Client &client, Channel* tarChannel);

		void	userLimitMode(Msg msg, Client &client, Channel *tarChannel);
	

		void	printChannels();
		void	printChannelUsers(Channel channel);

		int		nickClash(const std::string& nickname, int socket);
		

		//Getter
		int			getServerSocket() { return _serverSocket; }
		bool		getWelcomeSent() { return _welcomeSent; }
		std::string getPassword() { return _password; }
		std::string getServHostName() { return _servHostName; }
		std::string getStartTimeStr() { return _startTimeStr; }
};

	std::string getCurrentTime();
	std::string getCurrentEpochTime();
	time_t	stringToUnixTimeStamp(std::string time);
	int		getChannelIndex(std::string channel_name, std::vector<Channel> channel_names);
	void	initializeMsg(Msg &msg, std::vector<std::string> array);


	//User input checks
	int			checkArgumentCount(int argc);
	u_int16_t	getAndCheckPortValue(std::string port_string);
	int			passwordCheck(std::string password);

	//debuggers
	void	printMsg(Msg msg);
	void	printArray(std::vector<std::string>message_array);