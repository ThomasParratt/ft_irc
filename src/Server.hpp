
#ifndef SERVER_HPP
# define SERVER_HPP

#include "Msg.hpp"

class Server
{
	public:
		std::vector<Msg>			messages;

		void	makeMessages(std::vector<Msg> &msgs, std::string buffer);

		void	messageHandler(std::string buffer);
};

#endif