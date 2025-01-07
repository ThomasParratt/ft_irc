
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
		std::string		creationTime;

		int				user_limit;
		int 			OpCount;
		int				totalCount;
		bool			invite_only;
		bool			topic_requires_operator;

		std::vector<std::string>	invited;

		std::vector<User>	channel_users;

		Channel(std::string name);
		~Channel(){};

		bool				isChannelFull();
		bool				isChannelInviteOnly() {return invite_only; };
		bool				doesChannelHavePassword();

		void 				setChannelTopic(std::string new_topic, Client &client);	
		int					getNumberOfChannelUsers();
		int					getNumberOfChannelOperators();
	
		//Getters
		std::string			getChannelName(){return name;}
		std::string			getChannelKey(){return channel_key;}
		std::string 		getChannelTopic(){return topic;}
		std::string 		getTopicSetter(){return topicSetter;}
		std::string 		getTopicSetTime(){return topicSetTime;}
		std::string			getChannelTime(){return creationTime;}

		int					getUserLimit(){return user_limit;}
		int					getOpCount();
		int					getTotalCount();
		bool				getInviteOnly(){return invite_only;}
		bool				getTopicRequiresOperator(){return topic_requires_operator;}
		std::vector<User>	getChannelUsers(){return channel_users;}

		void				addUserIntoChannelUsers(User new_user);

	private:



};

#endif