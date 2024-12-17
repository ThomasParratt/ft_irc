
#include "Channel.hpp"
#include "Server.hpp"

Channel::Channel(std::string name) : 
		name					(name),
		user_limit				(-1),
		invite_only				(false),//TODO. Confirm this is default setting
		topic_requires_operator	(true)	//TODO. Confirm this is default setting
{
	;
}


void	Channel::setChannelTopic(std::string new_topic, Client &client)
{
	topic = new_topic;
	topicSetter = client.getNickname();
	topicSetTime = getCurrentTime();
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