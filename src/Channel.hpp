
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>//Where should this header go?
#include <vector>
#include "Client.hpp"

struct User
{
	std::string nickname;
	bool		operator_permissions;
	bool		topicSetter;
	bool	    invited;
	int 		socket; //user socket -> Is this needed?
};

class Channel
{
	public:
		std::string		name;
		std::string		channel_key;
		std::string 	topic;
		std::string		topicSetter;
		std::string 	topicSetTime;

		int				user_limit;
		bool			invite_only;
		bool			topic_requires_operator;

		std::vector<std::string>	invited;

		std::vector<User>	channel_users; // OR list of Clients???

		Channel(std::string name);

		bool				isChannelFull();
		bool				isChannelInviteOnly() {return invite_only; };
		bool				doesChannelHavePassword();

		//Getters
		std::vector<User>	getChannelUsers(){return channel_users;}

		std::string 		getChannelTopic(){return topic;}
		void 				setChannelTopic(std::string new_topic, Client &client);		

		int					getUserLimit(){return user_limit;}
		int					getNumberOfChannelUsers();
		std::string			getChannelKey(){return channel_key;}

	private:

};

#endif