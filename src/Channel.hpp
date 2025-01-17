#pragma once

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
	private:
		std::string		_name;
		std::string		_channel_key;
		std::string 	_topic;
		std::string		_topicSetter;
		std::string 	_topicSetTime;
		std::string		_creationTime;

		int				_user_limit;
		int 			_OpCount;
		int				_totalCount;
		bool			_keyRequired; // I added this as an extra safe for the channel
		bool			_invite_only;
		bool			_topic_requires_operator;

		std::vector<std::string>	_invited;
		std::vector<User>			_channel_users;


	public:

		Channel(std::string name);
		~Channel(){};

		bool				isChannelFull();
		bool				isChannelInviteOnly() {return _invite_only; };
		bool				doesChannelHavePassword();

		void 				setChannelTopic(std::string new_topic, Client &client);	
		int					getNumberOfChannelUsers();
		int					getNumberOfChannelOperators();
	
		//Getters
		std::string			getChannelName(){return _name;}
		std::string			getChannelKey(){return _channel_key;}
		std::string 		getChannelTopic(){return _topic;}
		std::string 		getTopicSetter(){return _topicSetter;}
		std::string 		getTopicSetTime(){return _topicSetTime;}
		std::string			getChannelTime(){return _creationTime;}

		int					getUserLimit(){return _user_limit;}
		int					getOpCount();
		int					getTotalCount();
		bool				getTopicRequiresOperator(){return _topic_requires_operator;}
		std::vector<std::string>&	getInvitedList(){return _invited;};
		std::vector<User>&	getChannelUsers(){return _channel_users;}
		User&				getChannelUserStruct(int index);

		std::string			getInvitedName(int index);
		void				addUserToInviteList(std::string nickname);
		void				uninviteUser(int index);

		void				setChannelKey(std::string key);
		void				setUserLimit(int userLimit);
		void				setKeyRequired(bool boolean);
		void				setInviteOnly(bool boolean);
		void				setTopicRequiresOperator(bool boolean);
		
		void				addUserIntoChannelUsers(User new_user);
		void				removeUserFromChannelUsers(int index);

};