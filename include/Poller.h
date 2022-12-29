#pragma once

#include "noncopyable.h"

#include <vector>
#include <unordered_map>

class Channel;
class EventLoop;
class TimeStamp;

class Poller : noncopyable {
public:
	using ChannelList = std::vector<Channel*>;
	
	Poller(EventLoop *loop);
	virtual ~Poller();

	virtual TimeStamp poll(int timeoutms, ChannelList *activeChannels) = 0;
	virtual void updateChannel(Channel*) = 0;
	virtual void removeChannel(Channel*) = 0;

	virtual bool hasChannel(Channel *channel) const;

	static Poller* newDefaultPoller(EventLoop *loop);
protected:
	using ChannelMap = std::unordered_map<int, Channel*>;
	ChannelMap channels_;
private:
	EventLoop *ownerLoop_;
};