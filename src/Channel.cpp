
#include "Channel.hpp"
#include "Server.hpp"

Channel::Channel(std::string name) : 
		_name						(name),
		_user_limit					(-1),
		_invite_only				(false),//TODO. Confirm this is default setting
		_keyRequired				(false),
		_topic_requires_operator	(true)	//TODO. Confirm this is default setting
{
	_creationTime = getCurrentTime();
}

int	Channel::getOpCount()
{
	int count = 0;
	for (std::vector<User>::iterator it = _channel_users.begin(); it != _channel_users.end(); it++)
	{
		if (it->operator_permissions)
			count++;
	}
	return count;
}

int		Channel::getTotalCount()
{
	return _channel_users.size();
}

User&	Channel::getChannelUserStruct(int index)
{
	return (this -> _channel_users[index]);
}

void Channel::setChannelKey(std::string key)
{
	this->_channel_key = key;
}

void	Channel::setChannelTopic(std::string new_topic, Client &client)
{
	this->_topic = new_topic;
	this->_topicSetter = client.getPrefix();
	this->_topicSetTime = getCurrentTime();
	std::cout << "Topic set by: " << _topicSetter << std::endl;
	std::cout << "Time: " << _topicSetTime << std::endl;
}

void	Channel::addUserIntoChannelUsers(User new_user)
{
	this->_channel_users.push_back(new_user);
}

void	Channel::removeUserFromChannelUsers(int index)
{
	if (index < 0)
	{
		return ;
	}
	this->_channel_users.erase(this ->_channel_users.begin() + index);
}

// std::string			Channel::getInvitedName(int index)
// {
// 	std::string		nickname;

// 	if (index < 0)
// 	{
// 		return ("");
// 	}

// 	nickname = this->invited[index];

// 	return (nickname);
// }

std::string Channel::getInvitedName(int index)
{
    if (index < 0 || index >= this->_invited.size())
    {
        return "";
    }

    return this->_invited[index];
}

void		Channel::addUserToInviteList(std::string nickname)
{
	this -> _invited.push_back(nickname);
}

void	Channel::uninviteUser(int index)
{
	if (index < 0)
	{
		return ;
	}
	std::cout << "Uninviting user 2" << std::endl;
	this->_invited.erase(this->_invited.begin() + index);
}

void	Channel::setKeyRequired(bool boolean)
{
	this -> _keyRequired = boolean;
}

void	Channel::setInviteOnly(bool boolean)
{
	this -> _invite_only = boolean;
}

void	Channel::setUserLimit(int userLimit)
{
	this -> _user_limit = userLimit;
}

void	Channel::setTopicRequiresOperator(bool boolean)
{
	this -> _topic_requires_operator = boolean;
}

/*
	14:27 -!- mkorpela [~mkorpela@194.136.126.51] has joined #ChanForChanny
	14:27 -!- ServerMode/#ChanForChanny [+Cnst] by iridium.libera.chat
	14:27 [Users #ChanForChanny]
	14:27 [@mkorpela] 
	14:27 -!- Irssi: #ChanForChanny: Total of 1 nicks [1 ops, 0 halfops, 0 voices, 0 normal]
	14:27 -!- Channel #ChanForChanny created Thu Dec  5 14:27:09 2024
	14:27 -!- Irssi: Join to #ChanForChanny was synced in 0 secs
	14:29 -!- shiii [~mkorpela@194.136.126.51] has joined #ChanForChanny
	14:29 < shiii> hi!
	14:29 <@mkorpela> hi Back!
	14:30 < shiii> byeee
	14:30 -!- shiii [~mkorpela@194.136.126.51] has left #ChanForChanny []

*/