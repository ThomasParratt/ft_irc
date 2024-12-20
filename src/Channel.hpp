
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

		std::vector<User>	channel_users;

		Channel(std::string name);

		bool				isChannelFull();
		bool				isChannelInviteOnly() {return invite_only; };
		bool				doesChannelHavePassword();

		void 				setChannelTopic(std::string new_topic, Client &client);	
		int					getNumberOfChannelUsers();
	
		//Getters
		std::string			getChannelName(){return name;}
		std::string			getChannelKey(){return channel_key;}
		std::string 		getChannelTopic(){return topic;}
		std::string 		getTopicSetter(){return topicSetter;}
		std::string 		getTopicSetTime(){return topicSetTime;}

		int					getUserLimit(){return user_limit;}
		bool				getInviteOnly(){return invite_only;}
		bool				getTopicRequiresOperator(){return topic_requires_operator;}
	
		std::vector<User>	getChannelUsers(){return channel_users;}

	private:

};

#endif