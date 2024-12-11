
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>//Where should this header go?
#include <vector>

struct User
{
	std::string nickname;
	bool		operator_permissions;
	//user socket -> Is this needed?
};

class Channel
{
	public:
		std::string		name;
		std::string		channel_key;
		std::string 	topic;

		int				user_limit;
		bool			invite_only;
		bool			topic_requires_operator;

		std::vector<User>	channel_users; // OR list of Clients???

		Channel(std::string name);


	private:

};

#endif