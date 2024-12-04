
#ifndef MSG_HPP
# define MSG_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#include <vector>



class Msg
{
	public:
		std::string					prefix;//Not really needed...
		std::string					command;
		std::vector<std::string>	parameters;
		std::string					trailing_msg;
};

#endif 