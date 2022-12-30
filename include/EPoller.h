#pragma once

#include <sys/epoll.h>

#include "Poller.h"

class EPoller : public Poller {
public:
	EPoller(EventLoop *loop);
	~EPoller() override;

	Timestamp poll(int timeoutms, ChannelList *activeChannels) override;
	void updateChannel(Channel*) override;
	void removeChannel(Channel*) override;
private:
	static const int kInitEventListSize = 16;

	void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
	void update(int operation, Channel *channel);

	using EventList = std::vector<epoll_event>;

	int epollfd_;
	EventList events_;
};